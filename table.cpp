#include "debug.h"
#include "database.h"

#include <QHostInfo>

Table::Table(QString table, Datum *value, Database *parent) :
	QObject((QObject*)parent)
{
	_tableName = table;
	_defaultValue = value;
	//_db = parent->

	Q_ASSERT(isValid());
}

Table::~Table(){

}

bool Table::isValid() const {

	bool success = _database->tableExists(_tableName);

	//Get datum field lists
	QMap<QString,QString> datumFields = _defaultValue->getFieldTypeMap();

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

	//TODO: Validate field types
	//Simplify type names
	//Map against datum fields


	return success;
}

QMap<QString,QString> Table::getColumnTypeMap() const {
	QMap<QString, QString> nameTypeMap;

	QString queryString = "PRAGMA table_info(locations)";
	QSqlQuery query(_db);

	if(doQuery(query, queryString)){
		/*Note: Available Fields {cid, name, type, notnull, dflt_value, pk}*/

		while(query.next()){
			QSqlRecord record = query.record();

			QString fieldName = record.value("name");
			QString fieldType = record.value("type");

			nameTypeMap.insert(fieldName, fieldType);
		}
	}

	return nameTypeMap;
}

QList<Datum*> Table::selectData(QList<Filter> filters, QString fieldList){
	//
}

QList<Datum*> Table::selectData(QList<Filter> filters, QList<int> requestedFields){
	//
}

QString Table::getTableName() const {

}


void Table::insertData(QList<Datum*> data, QList<int> fields){

}

void Table::updateData(QList<Datum*> data, QList<int> fields){

}
