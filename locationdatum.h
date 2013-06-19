#ifndef LOCATIONDATUM_H
#define LOCATIONDATUM_H

#include "database.h"

class LocationDatum : public Datum
{
    public:
        LocationDatum();

        virtual Datum* alloc();
        virtual QString getTypeName() const;
        virtual QString getFieldName(int index) const;
        virtual QVariant::Type getFieldType(int index) const;


        enum FieldPos{  //Id_Pos=0,
                        Timestamp_Pos=0,
                        DeviceId_Pos=1,
                        Source_Pos=2,
                        Latitude_Pos=3,
                        Longitude_Pos=4,
                        Hdop_Pos=5,
                        Elevation_Pos=6,
                        Course_Pos=7,
                        Speed_Pos=8,
                        Satelites_Pos=9  };


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

};

#endif // LOCATIONDATUM_H
