#include "debug.h"
#include "database.h"

#include <QHostInfo>

Database::Database(QString dbname, QSettings* settings, QObject *parent) :
	QObject(parent)
{
	_dbName = dbname;
	_dbconnectStr = "";
	_dbTypeStr = "QSQLITE";
	_host = QHostInfo::localHostName();

	_dbconnectStr = lookupDBPath(settings);
	qDebug() << dbname << " using db [" << _dbconnectStr << "]";

	bool success = openDB();
    if(!success){
        qCritical() << __PRETTY_FUNCTION__ << " - " << "Failure while opening duplocator database";
    }
	else{
		emit databaseOpened(_dbName, _dbconnectStr);
	}
}


QString Database::getDBName() const {
	return _dbName;
}


QString Database::getDBPath() const {
	return _dbconnectStr;
}

bool Database::tableExists(QString tableName) const {
	return _db.tables().contains(tableName);
}

QString Database::lookupDBPath(QSettings* settings){
	QString path = _dbconnectStr;

	if(path.isEmpty()){
		//QSettings settings("cumulonimbus", "nimbus-core");
		QString pathVar = _dbName.append("DbPath");

		if(settings->contains(pathVar)){
			QString dbPath = settings->value(pathVar).toString();
			if(dbPath.isEmpty()){
				qWarning() << "Empty path specified for " << _dbName << " in ["
						   << settings->fileName()
						   << "] dropping DB in user home";
				path = QDir::homePath();
			}
			else{
				path = dbPath;
			}
		}
		else{
			settings->setValue("firstRun", QVariant::fromValue<bool>(false));
			settings->sync();
			QFileInfo settingsFile(settings->fileName());

			QString settingsDir = settingsFile.canonicalPath();
			path = settingsDir.append(_dbName).append(".sqlite");

			QString pathVar = _dbName.append("DbPath");
			settings->setValue(pathVar, QVariant::fromValue<QString>(settingsDir));
			settings->sync();
		}
	}

	return path;
}


/*void Database::setDBPath(QString path){
	_dbconnectStr = path;
}*/


bool Database::openDB(){
	QString databaseHandle = _dbName.append("-datastore");
    if(QSqlDatabase::contains(databaseHandle)){
        _db = QSqlDatabase::database(databaseHandle);
    }
    else{
        _db = QSqlDatabase::addDatabase(_dbTypeStr, databaseHandle);
    }

    _db.setDatabaseName(_dbconnectStr);

    if(!_db.isOpen() && !_db.open()){
		qCritical() << "Failed to open " << _dbName << " database [" << _dbconnectStr << "]";
		return false;
	}

    QStringList tables = _db.tables(QSql::Tables);

	bool success = true;

    QString syncOff = "PRAGMA synchronous=OFF";
    QSqlQuery query(_db);

	bool setupSuccess = doQuery(query, syncOff);

	if(!setupSuccess){
		success = false;
		qCritical("Filed to configure database session");
	}


	QStringList tableNames = _db.tables(QSql::Tables);
	for(int i=0; i<tableNames.count(); i++){
		DEBUG_MSG() << "table[" << i << "] = " << tableNames[i];
	}

	return success;
}



bool Database::doQuery(QSqlQuery& query, QString queryString){
	Q_ASSERT( _db.isOpen() && _db.isValid() );
	bool success = query.exec(queryString);

	if(!success){
		qWarning() << "SqlQuery failed[type = " << query.lastError().type() << " text=" << query.lastError().text() << "] Offending string[" << queryString << "]";
		_dbError = query.lastError();
	}
    Q_ASSERT_X(success, "sql query error", qPrintable(query.lastError().text()));

	return success;
}




/**
 * @brief	Constructs a Table object if none exists already for the
 *			requested table. If the table does not exist in the database
 *			it will be initialized with all fields from the supplied
 *			example Datum. If an field named "id" exists it will be made
 *			the primary key. If the table already exists its fields will
 *			be compared against those found in the example Datum. Any
 *			missing fields will result in returning NULL and emitting an
 *			appropriate TableError.
 * @param tableName		Name of the table to return a pointer to
 * @param dataExample	An example of data stored in the requested table
 * @return	A pointer to the requested Table or NULL if an error was
 *			encountered.
 */
Table* Database::getTable(QString tableName, Datum* dataExample){
	if( !_db.tables(QSql::Tables).contains(tableName) ){
		DEBUG_MSG() << "No such table[" << tableName << "] in db[" << _dbName;
		return NULL;
	}


	if(_tables.contains(tableName)){
		return _tables.value(tableName);
	}

	Table* table = new Table(tableName, dataExample, this);

	if(table->isValid()){

		_tables.insert(tableName, table);

		return table;
	}

	return NULL;
}



