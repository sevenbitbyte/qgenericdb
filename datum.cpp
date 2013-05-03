#include "database.h"


Datum::Datum(QList<QVariant> defaultValues){
    for(int i=0; i<defaultValues.size(); i++){
        _valueList.push_back( new QVariant(defaultValues.at(i)) );
    }
}


QMap<QString,QString> Datum::getFieldTypeMap(){
    QMap<QString, QString> fieldMap;
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
