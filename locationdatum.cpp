#include "locationdatum.h"




LocationDatum::LocationDatum()
{
}


QString LocationDatum::getTypeName() const {
    return "LocationDatum";
}

QString LocationDatum::getFieldName(int index) const {
    switch(index){
        case 0:
            return QString("id");
        case 1:
            return QString("device_id");
        case 2:
            return QString("timestamp");
        case 3:
            return QString("source");
        case 4:
            return QString("ll_lat");
        case 5:
            return QString("ll_long");
        case 6:
            return QString("hdop");
        case 7:
            return QString("elev");
        case 8:
            return QString("course");
        case 9:
            return QString("speed");
        case 10:
            return QString("sats");
        default:
            break;
    }

    return QString("");
}


QVariant::Type LocationDatum::getFieldType(int index) const {
    switch(index){
        case 0:     //id
        case 1:     //device_id
        case 2:     //timestamp
        case 3:     //source
        case 10:    //sats
            return QVariant::LongLong;
        case 4:     //ll_lat
        case 5:     //ll_long
        case 6:     //hdop
        case 7:     //elev
        case 8:     //course
        case 9:     //speed
            return QVariant::Double;
        default:
            break;
    }

    return QVariant::Invalid;
}

int LocationDatum::deviceId() const {
    QVariant* devIdVariant = getValue(DeviceId_Pos);

    return devIdVariant->toInt();
}

void LocationDatum::setdeviceId(int id) {
    QVariant* devIdVariant = new QVariant( (qlonglong) id);

    setValue( (int) DeviceId_Pos, devIdVariant);
}

QDateTime LocationDatum::time() const {
    QVariant* timeVariant = getValue(Timestamp_Pos);

    QDateTime::fromMSecsSinceEpoch(timeVariant->toLongLong());
}

void LocationDatum::setTime(const QDateTime& time){
    QVariant* timeVariant = new QVariant( (qlonglong) time.toMSecsSinceEpoch() );

    Datum::setValue( (int) Timestamp_Pos, timeVariant);
}

LocationDatum::TrackSource LocationDatum::source() const {
    QVariant* sourceVariant = getValue(Source_Pos);

    Q_ASSERT( sourceVariant->toInt() < 3 );

    return (LocationDatum::TrackSource) sourceVariant->toInt();
}

void LocationDatum::setSource(TrackSource source){
    QVariant* sourceVariant = new QVariant( (qlonglong) source);

    setValue( (int) DeviceId_Pos, sourceVariant);
}

double LocationDatum::latitude() const {
    QVariant* latVariant = getValue(Latitude_Pos);

    return latVariant->toDouble();
}

void LocationDatum::setLatitude(double value){
    QVariant* latVariant = new QVariant( value );

    setValue( (int) Latitude_Pos, latVariant);
}

double LocationDatum::longitude() const{
    QVariant* longVariant = getValue(Longitude_Pos);

    return longVariant->toDouble();
}

void LocationDatum::setLongitude(double value) {
    QVariant* latVariant = new QVariant( value );

    setValue( (int) Latitude_Pos, latVariant);
}

double LocationDatum::hdop() const{
    QVariant* hdopVariant = getValue(Hdop_Pos);

    return hdopVariant->toDouble();
}

void LocationDatum::setHdop(double hdop){
    QVariant* hdopVariant = new QVariant( hdop );

    setValue( (int) Hdop_Pos, hdopVariant);
}

double LocationDatum::elevation() const {
    QVariant* elevVariant = getValue(Elevation_Pos);

    return elevVariant->toDouble();
}

void LocationDatum::setElevation(double elevation){
    QVariant* elevVariant = new QVariant( elevation );

    setValue( (int) Elevation_Pos, elevVariant);
}

double LocationDatum::course() const {
    QVariant* courseVariant = getValue(Course_Pos);

    return courseVariant->toDouble();
}

void LocationDatum::setCourse(double course){
    QVariant* courseVariant = new QVariant( course );

    setValue( (int) Course_Pos, courseVariant);
}

double LocationDatum::speed() const{
    QVariant* speedVariant = getValue(Speed_Pos);

    return speedVariant->toDouble();
}

void LocationDatum::setSpeed(double speed){
    QVariant* speedVariant = new QVariant( speed );

    setValue( (int) Speed_Pos, speedVariant);
}

int LocationDatum::satelites() const{
    QVariant* satVariant = getValue(Satelites_Pos);

    return satVariant->toInt();
}

void LocationDatum::setSatelites(int satelites){
    QVariant* satVariant = new QVariant( (qlonglong) satelites );

    setValue( (int) Satelites_Pos, satVariant);
}

