#include "debug.h"
#include "database.h"

#include <QHostInfo>

Table::Table(QString table, Datum *value, Database *parent) :
	QObject((QObject*)parent)
{
	_tableName = table;
	_exampleDatum = value;
	_database = parent;

	//TODO: setup _db

	Q_ASSERT(isValid());
}

Table::~Table(){
	//TODO:
	//	-Close _db
	//	-Free _defaultValue
}

bool Table::isValid() {

	bool success = _database->tableExists(_tableName);

	//Get datum field lists
    QMap<QString,QVariant::Type> datumFields = _exampleDatum->getFieldTypeMap();

	//Get table fields
    QMap<QString,QVariant::Type> tableColumns = getColumnTypeMap();


	//Locate different field names
	QSet<QString> differentFields = tableColumns.keys().toSet().subtract( datumFields.keys().toSet() );

	if(differentFields.size() != 0){
		DEBUG_MSG() << "Database table [" << _tableName << "] has " << differentFields.size() << " mismatched fields.";

		QString fieldName;
		foreach(fieldName, differentFields){

			if(datumFields.contains(fieldName)){
				DEBUG_MSG() << "Field [" << fieldName << "] only found in example datum";
			}
			else if(tableColumns.contains(fieldName)){
				DEBUG_MSG() << "Field [" << fieldName << "] only found in table";
			}
		}

		Q_ASSERT(false);
		return false;
	}


	//Retrieve field types
	QMap<QString,QVariant::Type> columnTypeMap = getColumnTypeMap();
	QMap<QString,QVariant::Type> datumTypeMap = _exampleDatum->getFieldTypeMap();

	//TODO: Simplify type names

	//Validate types
	QMap<QString,QVariant::Type> typeMap = columnTypeMap.unite(datumTypeMap);
	QList<QString> fieldNames = typeMap.uniqueKeys();


	//Verify we have two entries per fieldName
	for(int i=0; i<fieldNames.length(); i++){
		QString name = fieldNames[i];

		//Field present in database table?
		if(!columnTypeMap.contains(name)){
			DEBUG_MSG() << "Field[" << name << "] not present in existing table["
					  << _tableName << "]";

			success = false;
			break;
		}

		if(!datumTypeMap.contains(name)){
			DEBUG_MSG() << "Field[" << name << "] not present in supplied datum["
					  << _exampleDatum->getTypeName() << "]";
			success = false;
			break;
		}

		QList<QVariant::Type> typeList = typeMap.values(name);

		if(typeList.length() != 2){
			DEBUG_MSG() << "Expected exactly 2 items but recieved " << typeList.length();
			success = false;
			break;
		}

		if(typeList.at(0) != typeList.at(1)){
			DEBUG_MSG() << "Type names do not match(" << typeList.at(2) << ", " << typeList.at(1);
			success = false;
			break;
		}
	}

	return success;
}

QMap<QString,QVariant::Type> Table::getColumnTypeMap()  {
    QMap<QString, QVariant::Type> nameTypeMap;

    QString queryString = QString("PRAGMA table_info(%1)").arg(getTableName());
	QSqlQuery query(_db);

	if(doQuery(query, queryString)){
		/*Note: Available Fields {cid, name, type, notnull, dflt_value, pk}*/

		while(query.next()){
			QSqlRecord record = query.record();

			QString fieldName = record.value("name").toString();
            QString fieldType = record.value("type").toString().toLower();

            QVariant::Type variantType = QVariant::Invalid;

            if(fieldType.contains("int")){
                variantType = QVariant::LongLong;
            }
            else if(fieldType.contains("real")){
                variantType = QVariant::Double;
            }
            else{
                qCritical() << QString("Failed to determine proper type label for field [%1,%2]").arg(fieldName).arg(fieldType);
            }

            nameTypeMap.insert(fieldName, variantType);
		}
	}

	return nameTypeMap;
}

bool Table::doQuery(QSqlQuery& query, QString queryString){
    bool success = query.exec(queryString);

    if(!success){
        qWarning() << "SqlQuery failed[type = " << query.lastError().type() << " text=" << query.lastError().text() << "] Offending string[" << queryString << "]";
        //_dbError = query.lastError();
    }
    Q_ASSERT_X(success, "sql query error", qPrintable(query.lastError().text()));

    return success;
}

template<typename DatumType>  QList<Datum*> Table::selectDataSync(QString queryStr){
    QList<Datum*> resultList;
    QSqlQuery query(_db);



    if( doQuery(query, queryStr) ){
        while( query.next() ){
            QSqlRecord record = query.record();

            Datum* datum = new DatumType();

            //Copy field values
            for(int i=0; i<datum->getFieldCount(); i++){
                int fieldIdx = datum->getFieldName(i);

                datum->setValue(i, record.value(fieldIdx));
            }

            //Get rowId field's index
            int rowIdIdx = record.indexOf("rowid");
            Q_ASSERT(rowIdIdx > -1);
            //Set datum rowId
            datum->setId(record.value(rowIdIdx).LongLong());
        }
    }

    return resultList;
}



QString Table::getTableName() const {
	return _tableName;
}

void Table::insertData(QList<Datum*> data, QList<int> fields){
    bool success = insertDataSync( data, fields);
    Q_ASSERT(success);
}


bool Table::insertDataSync(QList<Datum*> data, QList<int> fields){
    QSqlQuery query(_db);
    QString queryString;
    QTextStream stringStream(&queryString);

    stringStream << "INSERT INTO " << _tableName;

    if(!fields.isEmpty()){
         stringStream << "(";

        //Build list of affected columns
        for(int i=0; i<fields.size(); i++){
            stringStream << _exampleDatum->getFieldName(i);

            if(i < fields.size() - 1){
                stringStream << ", ";
            }
        }

        stringStream << ")";
    }

    stringStream << " VALUES (";

    for(int i=0; i<data.size(); i++){
        stringStream << "?";
        if(i < data.size() - 1){
            stringStream << ", ";
        }
    }

    stringStream << ")";
    query.prepare(queryString);

    //Construct bind lists
    for(int i=0; i<fields.size(); i++){
        QVariantList values;

        for(int j=0; j<data.size(); j++){
            values.push_back( *data[j]->getValue(fields[i]) );
        }

        query.addBindValue(values);
    }

    return query.execBatch();
}

void Table::updateData(QList<Datum*> data, QList<int> fields, QList<int> matchOn){
    bool success = updateDataSync(data, fields, matchOn);

    Q_ASSERT(success);
}

bool Table::updateDataSync(QList<Datum*> data, QList<int> fields, QList<int> matchOn){
    QSqlQuery query(_db);
    QString queryString;
    QTextStream stringStream(&queryString);

    //UPDATE TEST_TABLE SET COL1 = ?, COL2 = ? WHERE ID = ?

    stringStream << "UPDATE " << _tableName << " SET ";

    if(fields.empty()){
        //Build list of all datum field indexes
        for(int i=0; i<_exampleDatum->getFieldCount(); i++){
            fields.push_back(i);
        }
    }

    //Construct SET field list fragment
    for(int i=0; i<fields.size(); i++){
        stringStream << _exampleDatum->getFieldName( fields[i] ) << " = ?";

        if(i < fields.size() - 1){
            stringStream << ", ";
        }
    }

    stringStream << " WHERE ";
    if(matchOn.isEmpty()){
        //Match on rowId
        stringStream << "rowid = ?";
    }
    else{
        //Construct WHERE fragment
        for(int i=0; i<matchOn.size(); i++){
            stringStream << _exampleDatum->getFieldName( matchOn[i] ) << " = ?";

            if(i < matchOn.size() - 1){
                stringStream << " AND ";
            }
        }
    }

    stringStream << ")";
    query.prepare(queryString);

    //Bind values to set
    for(int i=0; i<fields.size(); i++){
        QVariantList values;

        for(int j=0; j<data.size(); j++){
            values.push_back( *data[j]->getValue(fields[i]) );
        }

        query.addBindValue(values);
    }

    //Bind match values
    if(matchOn.isEmpty()){
        //Bind on rowids
        QVariantList values;

        for(int j=0; j<data.size(); j++){
            values.push_back( data[j]->id() );
        }

        query.addBindValue(values);
    }
    else{
        for(int i=0; i<matchOn.size(); i++){
            QVariantList values;

            for(int j=0; j<data.size(); j++){
                values.push_back( *data[j]->getValue(matchOn[i]) );
            }

            query.addBindValue(values);
        }
    }

    return query.execBatch();
}
