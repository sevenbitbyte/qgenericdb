#include "database.h"


Datum::Datum(QList<QVariant> defaultValues){
    for(int i=0; i<defaultValues.size(); i++){
        _valueList.push_back( new QVariant(defaultValues.at(i)) );
    }
}


QMap<QString,QVariant::Type> Datum::getFieldTypeMap() const {
    QMap<QString, QVariant::Type> fieldMap;
    QList<QString> fieldNames = getFieldNames();

    for(int i=0; i < fieldNames.count(); i++){
        fieldMap.insert( fieldNames[i], getFieldType(i) );
    }

    return fieldMap;
}

qint32 Datum::getFieldCount() const {
    return _valueList.count();
}

QList<QVariant*> Datum::getValues() const {
    return _valueList;
}

QList<QString> Datum::getFieldNames() const {
    QList<QString> fieldNameList;

    for(int i=0; i < getFieldCount(); i++){
        fieldNameList.push_back( getFieldName(i) );
    }

    return fieldNameList;
}

void Datum::setValue(int index, QVariant& value){
    Q_ASSERT(getFieldType(index) == value.type);

    _valueList.replace(index, new QVariant(value));
}

void Datum::setValue(int index, QVariant* value){
	Q_ASSERT(getFieldType(index) == value.type);

    _valueList.replace(index, value);
}


QVariant* Datum::getValue(int index) const {
    if(index > _valueList.size()-1 || index < 0){
        Q_ASSERT(false);
        return NULL;
    }

    return _valueList.at(index);
}

qlonglong Datum::id(){
    QVariant* idVariant = getValue(0);

    return idVariant->toLongLong();
}
