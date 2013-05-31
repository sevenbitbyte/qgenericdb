#include "debug.h"
#include "database.h"

#include <QHostInfo>

Database::Database(QString dbname, QSettings* settings, QObject *parent) :
	QObject(parent)
{
	_dbName = dbname;
    _dbconnectStr = QString();
	_dbTypeStr = "QSQLITE";
    //_host = QHostInfo::localHostName();

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
		QString pathVar = _dbName.append("DbPath");

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
                path = settingsDir.append(_dbName).append(".sqlite");

                //Save db path to application settings
                QString firstRunVar = pathVar.append("FirstRun");
                settings->setValue(pathVar, QVariant::fromValue<QString>(settingsDir));
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
            QString firstRunVar = pathVar.append("FirstRun");
            settings->setValue(firstRunVar, QVariant::fromValue<bool>(true));
			settings->sync();

            //Determine location of application settings
			QFileInfo settingsFile(settings->fileName());

            //Construct db path residing in same directory as app settings
			QString settingsDir = settingsFile.canonicalPath();
			path = settingsDir.append(_dbName).append(".sqlite");

            //Save db path to application settings
			settings->setValue(pathVar, QVariant::fromValue<QString>(settingsDir));
            settings->setValue(firstRunVar, QVariant::fromValue<bool>(false));
			settings->sync();
		}
	}

	return path;
}


bool Database::openDB(){
	QString databaseHandle = _dbName.append("-datastore");

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
Table* Database::getTable(Datum* dataExample, QString tableName){
    if(tableName.isEmpty()){
        tableName = dataExample->getTypeName();
    }

	if( !_db.tables(QSql::Tables).contains(tableName) ){
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

    QMap<QString,QVariant::Type> fieldTypeMap = dataExample->getFieldTypeMap();
    QMap<QString,QVariant::Type>::Iterator fieldIter = fieldTypeMap.begin();

    while(fieldIter != fieldTypeMap.end()){

        queryStream << fieldIter.key() << " " << Database::variantToSqlType(fieldIter.value());

        if(fieldIter == fieldTypeMap.begin()){
            //Make first field primary key
            queryStream << "PRIMARY KEY";

            if(Database::variantToSqlType(fieldIter.value()) == "INTEGER"){
                queryStream << " AUTOINCREMENT";
            }
        }

        fieldIter++
        if(fieldIter != fieldTypeMap.end()){
            queryStream << ",";
        }
    }

    queryStream << ")";

    QSqlQuery query(_db);
    return doQuery(query, queryStr);
}

