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
    

    QDateTime selectStartTime = QDateTime::currentDateTimeUtc();
    QList<Datum*> data = locationTable->selectDataSync();
    QDateTime selectEndTime = QDateTime::currentDateTimeUtc();

    qint64 selectDeltaMs = selectStartTime.msecsTo(selectEndTime);

    qDebug() << "Selected " << data.size() << " elements in " << selectDeltaMs << "ms";

    for(int i=0; i<data.size(); i++){
        LocationDatum* datum = (LocationDatum*) data[i];
        datum->setElevation( datum->elevation() + 5 );
    }

    if(data.size() < 1000000){
        QList<Datum*> positions;

        double latitude = 0;
        double longitude = 0;

        for(int i=0; i<100000; i++){
            LocationDatum* datum = new LocationDatum();

            datum->setSource(LocationDatum::Source_Gps);
            datum->setHdop(3.0f);

            datum->setLatitude( latitude++ );
            datum->setLongitude( longitude++ );

            positions.append(datum);
        }

        QDateTime startTime = QDateTime::currentDateTimeUtc();
        locationTable->insertDataSync(positions);
        QDateTime endTime = QDateTime::currentDateTimeUtc();

        qint64 deltaMs = startTime.msecsTo(endTime);

        qDebug() << "Inserted " << positions.size() << " elements in " << deltaMs << "ms";
    }
    else{
        QDateTime updateStartTime = QDateTime::currentDateTimeUtc();
        if(locationTable->updateDataSync(data)){
            QDateTime updateEndTime = QDateTime::currentDateTimeUtc();

            qint64 updateDeltaMs = updateStartTime.msecsTo(updateEndTime);

            qDebug() << "Updated " << data.size() << " elements in " << updateDeltaMs << "ms";
        }
        else{
            qCritical() << "Failed to update data";
        }
    }

    return 0;
}
