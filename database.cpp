#include "debug.h"
#include "database.h"

#include <QHostInfo>

Database::Database(QString dbname, QSettings* settings, QObject *parent) :
	QObject(parent)
{
	_dbName = dbname;
    _dbconnectStr = QString();
    _dbTypeStr = "QSQLITE";

	_dbconnectStr = lookupDBPath(settings);
    qDebug() << _dbName << " using db [" << _dbconnectStr << "]";

	bool success = openDB();
    if(!success){
        qCritical() << __PRETTY_FUNCTION__ << " - " << "Failure while opening " << _dbName << " database located at " << _dbconnectStr;
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
        QString pathVar = QString(_dbName).append("DbPath");

        //Check for db path in application settings
		if(settings->contains(pathVar)){
            //Read db path from app settings
			QString dbPath = settings->value(pathVar).toString();
			if(dbPath.isEmpty()){
                //Invalid path, default to app settings directory

                //Determine location of application settings
                QFileInfo settingsFile(settings->fileName());

                //Construct db path residing in same directory as app settings
                QString settingsDir = settingsFile.canonicalPath();
                path = QString(settingsDir).append(_dbName).append(".sqlite");

                //Save db path to application settings
                QString firstRunVar = QString(pathVar).append("FirstRun");
                settings->setValue(pathVar, QVariant::fromValue<QString>(path));
                settings->setValue(firstRunVar, QVariant::fromValue<bool>(false));
                settings->sync();
			}
			else{
				path = dbPath;
			}
		}
        else{
            //Db path not defined in app settings
            //Set firstrun flag to true
            QString firstRunVar = QString(pathVar).append("FirstRun");
            settings->setValue(firstRunVar, QVariant::fromValue<bool>(true));
			settings->sync();

            //Determine location of application settings
			QFileInfo settingsFile(settings->fileName());

            //Construct db path residing in same directory as app settings
			QString settingsDir = settingsFile.canonicalPath();
            path = QString(settingsDir).append("/").append(_dbName).append(".sqlite");

            //Save db path to application settings
            settings->setValue(pathVar, QVariant::fromValue<QString>(path));
            settings->setValue(firstRunVar, QVariant::fromValue<bool>(false));
			settings->sync();
		}
	}

	return path;
}


bool Database::openDB(){
    QString databaseHandle = _dbName;

    if(QSqlDatabase::contains(databaseHandle)){
        //Use existing db connection
        _db = QSqlDatabase::database(databaseHandle);
    }
    else{
        //Create new db connection
        _db = QSqlDatabase::addDatabase(_dbTypeStr, databaseHandle);
    }

    _db.setDatabaseName(_dbconnectStr);

    if(!_db.isOpen() && !_db.open()){
		qCritical() << "Failed to open " << _dbName << " database [" << _dbconnectStr << "]";
		return false;
	}

	bool success = true;

    QString syncOff = "PRAGMA synchronous=OFF";
    QSqlQuery query(_db);

	bool setupSuccess = doQuery(query, syncOff);

	if(!setupSuccess){
		success = false;
        qCritical() << "Failed to configure database[" << _dbName << "] session";
	}


	QStringList tableNames = _db.tables(QSql::Tables);
	for(int i=0; i<tableNames.count(); i++){
        DEBUG_MSG() << _dbName << ":table[" << i << "] = " << tableNames[i];
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

Table* Database::getTable(Datum* dataExample, QString tableName){
    if(tableName.isEmpty()){
        tableName = dataExample->getTypeName();
    }

    if(!_db.tables(QSql::Tables).contains(tableName)){
        DEBUG_MSG() << "No such table[" << tableName << "] in db[" << _dbName << "]";
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

QString Database::variantToSqlType(QVariant::Type variantType){
    QString typeStr = QString();
    switch(variantType){
        case QVariant::LongLong:
            typeStr = "INTEGER";
            break;
        case QVariant::Double:
            typeStr = "REAL";
            break;
        case QVariant::String:
            typeStr = "TEXT";
            break;
        case QVariant::ByteArray:
            typeStr = "BLOB";
            break;
        default:
            qCritical() << "Unsupported variant[" << variantType << "] requested";
            break;
    }

    return typeStr;
}

bool Database::createTable(Datum* dataExample, QString tableName){
    if(tableName.isEmpty()){
        tableName = dataExample->getTypeName();
    }


    QString queryStr;
    QTextStream queryStream(&queryStr);

    queryStream << "CREATE TABLE IF NOT EXISTS " << tableName << "(";

    QList<QString> fieldList = dataExample->getFieldNames();

    for(int i=0; i < fieldList.size(); i++){
        QVariant::Type fieldType = dataExample->getFieldType(i);

        queryStream << fieldList[i] << " " << Database::variantToSqlType( fieldType );

        if(i==0){
            //Make first field primary key
            queryStream << " PRIMARY KEY";

            if(Database::variantToSqlType(fieldType) == "INTEGER" && fieldList[i].toLower() == "id"){
                queryStream << " AUTOINCREMENT";
            }
        }

        if(i != fieldList.size() - 1){
            queryStream << ", ";
        }
    }

    queryStream << ")";

    qDebug() << "Creating table using " << queryStr;

    QSqlQuery query(_db);
    return doQuery(query, queryStr);
}

