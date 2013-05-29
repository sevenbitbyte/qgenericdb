#include "locationdatum.h"




LocationDatum::LocationDatum()
{
    _datum = this;
}

/*LocationDatum::LocationDatum(Datum *parent){
    _datum = parent;
}*/

QString LocationDatum::getTypeName() const {
    return "LocationDatum";
}

QString LocationDatum::getFieldName(int index) const {
    switch(index){
        case Id_Pos:
            return QString("id");
        case DeviceId_Pos:
            return QString("device_id");
        case Timestamp_Pos:
            return QString("timestamp");
        case Source_Pos:
            return QString("source");
        case Latitude_Pos:
            return QString("ll_lat");
        case Longitude_Pos:
            return QString("ll_long");
        case Hdop_Pos:
            return QString("hdop");
        case Elevation_Pos:
            return QString("elev");
        case Course_Pos:
            return QString("course");
        case Speed_Pos:
            return QString("speed");
        case Satelites_Pos:
            return QString("sats");
        default:
            break;
    }

    return QString("");
}


QVariant::Type LocationDatum::getFieldType(int index) const {
    switch(index){
        case Id_Pos:
        case DeviceId_Pos:
        case Timestamp_Pos:
        case Source_Pos:
        case Satelites_Pos:
            return QVariant::LongLong;
        case Latitude_Pos:
        case Longitude_Pos:
        case Hdop_Pos:
        case Elevation_Pos:
        case Course_Pos:
        case Speed_Pos:
            return QVariant::Double;
        default:
            break;
    }

    return QVariant::Invalid;
}

int LocationDatum::deviceId() const {
    QVariant* devIdVariant = _datum->getValue(DeviceId_Pos);

    return devIdVariant->toInt();
}

void LocationDatum::setdeviceId(int id) {
    QVariant* devIdVariant = new QVariant( (qlonglong) id);

    _datum->setValue( (int) DeviceId_Pos, devIdVariant);
}

QDateTime LocationDatum::time() const {
    QVariant* timeVariant = _datum->getValue(Timestamp_Pos);

    return QDateTime::fromMSecsSinceEpoch(timeVariant->toLongLong());
}

void LocationDatum::setTime(const QDateTime& time){
    QVariant* timeVariant = new QVariant( (qlonglong) time.toMSecsSinceEpoch() );

    _datum->setValue( (int) Timestamp_Pos, timeVariant);
}

LocationDatum::TrackSource LocationDatum::source() const {
    QVariant* sourceVariant = _datum->getValue(Source_Pos);

    Q_ASSERT( sourceVariant->toInt() < 3 );

    return (LocationDatum::TrackSource) sourceVariant->toInt();
}

void LocationDatum::setSource(TrackSource source){
    QVariant* sourceVariant = new QVariant( (qlonglong) source);

    _datum->setValue( (int) DeviceId_Pos, sourceVariant);
}

double LocationDatum::latitude() const {
    QVariant* latVariant = _datum->getValue(Latitude_Pos);

    return latVariant->toDouble();
}

void LocationDatum::setLatitude(double value){
    QVariant* latVariant = new QVariant( value );

    _datum->setValue( (int) Latitude_Pos, latVariant);
}

double LocationDatum::longitude() const{
    QVariant* longVariant = _datum->getValue(Longitude_Pos);

    return longVariant->toDouble();
}

void LocationDatum::setLongitude(double value) {
    QVariant* latVariant = new QVariant( value );

    _datum->setValue( (int) Latitude_Pos, latVariant);
}

double LocationDatum::hdop() const{
    QVariant* hdopVariant = _datum->getValue(Hdop_Pos);

    return hdopVariant->toDouble();
}

void LocationDatum::setHdop(double hdop){
    QVariant* hdopVariant = new QVariant( hdop );

    _datum->setValue( (int) Hdop_Pos, hdopVariant);
}

double LocationDatum::elevation() const {
    QVariant* elevVariant = _datum->getValue(Elevation_Pos);

    return elevVariant->toDouble();
}

void LocationDatum::setElevation(double elevation){
    QVariant* elevVariant = new QVariant( elevation );

    _datum->setValue( (int) Elevation_Pos, elevVariant);
}

double LocationDatum::course() const {
    QVariant* courseVariant = _datum->getValue(Course_Pos);

    return courseVariant->toDouble();
}

void LocationDatum::setCourse(double course){
    QVariant* courseVariant = new QVariant( course );

    _datum->setValue( (int) Course_Pos, courseVariant);
}

double LocationDatum::speed() const{
    QVariant* speedVariant = _datum->getValue(Speed_Pos);

    return speedVariant->toDouble();
}

void LocationDatum::setSpeed(double speed){
    QVariant* speedVariant = new QVariant( speed );

    _datum->setValue( (int) Speed_Pos, speedVariant);
}

int LocationDatum::satelites() const{
    QVariant* satVariant = _datum->getValue(Satelites_Pos);

    return satVariant->toInt();
}

void LocationDatum::setSatelites(int satelites){
    QVariant* satVariant = new QVariant( (qlonglong) satelites );

    _datum->setValue( (int) Satelites_Pos, satVariant);
}

