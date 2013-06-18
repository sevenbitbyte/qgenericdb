#ifndef LOCATIONDATUM_H
#define LOCATIONDATUM_H

#include "database.h"

class LocationDatum : public Datum
{
    public:
        LocationDatum();
        //LocationDatum(Datum* parent);

        virtual QString getTypeName() const;
        //virtual qint32 getFieldCount() const;
        virtual QString getFieldName(int index) const;
        virtual QVariant::Type getFieldType(int index) const;


        enum FieldPos{  Id_Pos=0,
                        DeviceId_Pos=1,
                        Timestamp_Pos=2,
                        Source_Pos=3,
                        Latitude_Pos=4,
                        Longitude_Pos=5,
                        Hdop_Pos=6,
                        Elevation_Pos=7,
                        Course_Pos=8,
                        Speed_Pos=9,
                        Satelites_Pos=10  };


        int deviceId() const;
        void setdeviceId(int id);

        QDateTime time() const;
        void setTime(const QDateTime& time);

        enum TrackSource{ Source_Unknown=0, Source_Gps, Source_Network };
        TrackSource source() const;
        void setSource(TrackSource source);

        double latitude() const;
        void setLatitude(double value);

        double longitude() const;
        void setLongitude(double value);

        double hdop() const;
        void setHdop(double hdop);

        double elevation() const;
        void setElevation(double elevation);

        double course() const;
        void setCourse(double course);

        double speed() const;
        void setSpeed(double speed);

        int satelites() const;
        void setSatelites(int satelites);

    private:
        //Datum* _datum;
};

#endif // LOCATIONDATUM_H
