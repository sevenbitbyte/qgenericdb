#include "database.h"


Datum::Datum(){
    //Do nothing
    _rowId = -1;
}

/*Datum::Datum(QList<QVariant> defaultValues){
    unsetValues();
    _rowId = -1;

    for(int i=0; i<defaultValues.size(); i++){
        _valueList.push_back( new QVariant(defaultValues.at(i)) );
    }
}


Datum::Datum(QMap<QString, QVariant> initialValues){
    unsetValues();
    _rowId = -1;
    QList<QString> fieldNames = getFieldNames();

    QMap<QString,QVariant>::iterator iter;
    for(iter = initialValues.begin(); iter != initialValues.end(); iter++){
        int idx = fieldNames.indexOf( iter.key() );

        setValue(idx, iter.value());
    }
}*/

QMap<QString,QVariant::Type> Datum::getFieldTypeMap() const {
    QMap<QString, QVariant::Type> fieldMap;
    QList<QString> fieldNames = getFieldNames();

    for(int i=0; i < fieldNames.count(); i++){
        fieldMap.insert( fieldNames[i], getFieldType(i) );
    }

    return fieldMap;
}

qint32 Datum::getFieldCount() const {
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

    _fieldCount = types.size();

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

qlonglong Datum::id() const {
   return _rowId;
}

void Datum::setId(qlonglong value){
    _rowId = value;
}
