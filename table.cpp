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
	QMap<QString,QString> datumFields = _exampleDatum->getFieldTypeMap();

	//Get table fields
	QMap<QString,QString> tableColumns = getColumnTypeMap();


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
	QMap<QString,QString> columnTypeMap = getColumnTypeMap();
	QMap<QString,QString> datumTypeMap = _exampleDatum->getFieldTypeMap();

	//TODO: Simplify type names

	//Validate types
	QMap<QString,QString> typeMap = columnTypeMap.unite(datumTypeMap);
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

		QList<QString> typeList = typeMap.values(name);

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

QMap<QString,QString> Table::getColumnTypeMap()  {
	QMap<QString, QString> nameTypeMap;

	QString queryString = "PRAGMA table_info(locations)";
	QSqlQuery query(_db);

	if(doQuery(query, queryString)){
		/*Note: Available Fields {cid, name, type, notnull, dflt_value, pk}*/

		while(query.next()){
			QSqlRecord record = query.record();

			QString fieldName = record.value("name").toString();
			QString fieldType = record.value("type").toString();

			nameTypeMap.insert(fieldName, fieldType);
		}
	}

	return nameTypeMap;
}

bool Table::doQuery(QSqlQuery& query, QString queryString){
	return false;
}

QList<Datum*> Table::selectData(QList<Filter> filters, QString fieldList){
	//
}

QList<Datum*> Table::selectData(QList<Filter> filters, QList<int> requestedFields){
	//
}

QString Table::getTableName() const {
	return _tableName;
}


void Table::insertData(QList<Datum*> data, QList<int> fields){

}

void Table::updateData(QList<Datum*> data, QList<int> fields){

}
