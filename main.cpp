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

    Table* locationTable = db.getTable(&exampleLoc);

    if(locationTable == NULL){
        qCritical() << "Got null reference to location table";
    }

    QList<Datum*> positions;

    exampleLoc.setSource(LocationDatum::Source_Gps);
    exampleLoc.setHdop(3.0f);

    positions.append(&exampleLoc);
    positions.append(&exampleLoc);
    positions.append(&exampleLoc);

    locationTable->insertDataSync(positions);
    
    QList<Datum*> data = locationTable->selectDataSync();

    for(int i=0; i<data.size(); i++){
        Datum* datum = data[i];

        QString output;
        QTextStream outStream(&output);

        outStream << datum->id() << ": ";

        for(int j=0; j<datum->getFieldCount(); j++){
            outStream  << datum->getFieldName(j) << "=" << datum->getValue(j)->toString() << " ";
        }

        qDebug() << output;
    }


    return 0;
	
    //return a.exec();
}
