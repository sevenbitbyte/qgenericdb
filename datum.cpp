#include "database.h"


Datum::Datum(){
    //Do nothing
    _rowId = -1;
    _fieldCount = -1;
}


QMap<QString,QVariant::Type> Datum::getFieldTypeMap() const {
    QMap<QString, QVariant::Type> fieldMap;
    QList<QString> fieldNames = getFieldNames();

    for(int i=0; i < fieldNames.count(); i++){
        fieldMap.insert( fieldNames[i], getFieldType(i) );
    }

    return fieldMap;
}

qint32 Datum::getFieldCount() {
    if(_fieldCount == -1){
        _fieldCount = getFieldNames().size();
    }

    return _fieldCount;
}

QList<QVariant*> Datum::getValues() const {
    return _valueList;
}

QList<QString> Datum::getFieldNames() const {
    QList<QString> fieldNameList;

    int i = 0;
    QString fieldName = getFieldName(i);

    while( !fieldName.isEmpty() ){
        fieldNameList.push_back( fieldName );

        fieldName = getFieldName(++i);
    }

    return fieldNameList;
}

void Datum::setValue(int index, QVariant& value){
    Q_ASSERT(getFieldType(index) == value.type());

    setValue(index, new QVariant(value));
}

void Datum::setValue(int index, QVariant* value){
    Q_ASSERT(getFieldType(index) == value->type());

    if(_valueList.at(index) == NULL){
        _valueList.replace(index, value);
    }
    else{
        QVariant* variant = _valueList.at(index);
        (*variant) = *value;
    }
}


QVariant* Datum::getValue(int index) const {
    if(index > _valueList.size()-1 || index < 0){
        //Index does not exist
        Q_ASSERT(false);
        return NULL;
    }

    return _valueList.at(index);
}

bool Datum::isSet(int index) const {
    return (getValue(index) != NULL);
}

void Datum::unsetValue(int index){
    if(_valueList.at(index) != NULL){
        //Free underlaying data
        delete _valueList.at(0);

        _valueList.replace(index, NULL);
    }
}

void Datum::unsetValues(){
    QList<QVariant::Type> types = getFieldTypeMap().values();

    for(int i=0; i<types.size(); i++){
        if(_valueList.size() < (i+1)){
            _valueList.push_back(NULL);
        }
        else if(_valueList.at(i) != NULL){
            delete _valueList.first();
            _valueList.replace(i, NULL);
        }
    }
}

void Datum::initValues(){
    QList<QVariant::Type> types = getFieldTypeMap().values();

    for(int i=0; i<types.size(); i++){
        QVariant* value = new QVariant(types[i]);

        if(_valueList.size() < (i+1)){
            _valueList.push_back(value);
        }
        else if(_valueList.at(i) != NULL){
            delete _valueList.first();
            _valueList.replace(i, value);
        }
    }
}

qlonglong Datum::id() const {
   return _rowId;
}

void Datum::setId(qlonglong value){
    _rowId = value;
}
