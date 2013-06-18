#include <QCoreApplication>
#include <QtCore>

#include "database.h"
#include "locationdatum.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

    QSettings settings("cumulonimbus", "nimbus-core");

    Database db("location", &settings, &a);

    LocationDatum exampleLoc;

    qDebug() << "Type[" << exampleLoc.getTypeName() << "] FieldCount=" << exampleLoc.getFieldCount();

    if( !db.tableExists(exampleLoc.getTypeName()) ){
        qWarning() << "Table[" <<  exampleLoc.getTypeName() << "] does not exist";

        if(db.createTable(&exampleLoc)){
            qDebug() << "Created table for type [" << exampleLoc.getTypeName() <<"]";
            //return 0;
        }
        else{
            qCritical() << "Failed to create table for type [" << exampleLoc.getTypeName() <<"]";
            return -1;
        }
    }
    else{
        qWarning() << "Table[" <<  exampleLoc.getTypeName() << "] already exist";
    }

    return 0;
	
    //return a.exec();
}
