#ifndef DATABASE_H
#define DATABASE_H

#include <QtCore>
#include <QtSql>


class Datum{
	public:
        /**
         * @brief Construct a Datum with default values. Expects all fields to be supplied
         * @param defaultValues
         */
		Datum(QList<QVariant> defaultValues=QList<QVariant>());

        /**
         * @brief Construct a Datum with the specified intial values
         * @param initialValues
         */
        Datum(QMap<QString,QVariant> initialValues);


		/**
         * @brief	Returns a mapping of indexes to field names
         * @return
         */
        QMap<QString,QVariant::Type> getFieldTypeMap() const;
        qint32 getFieldCount() const;

        QList<QVariant*> getValues() const;
        QList<QString> getFieldNames() const;

        /**
         * @brief Assigns the supplied value to the specified index
         * @param index
         * @param value
         */
        void setValue(int index, QVariant& value);
        void setValue(int index, QVariant* value);

        /**
         * @brief   Returns a pointer to the underlaying variant for the
         *          at the specified index.
         * @param   index
         * @return  Returns a point to the underylaying variant, this can be NULL.
         */
        QVariant* getValue(int index) const;

        bool isSet(int index) const;

        /**
         * @brief   Sets variant pointer to NULL to indicate that no value
         *          is set. Operationally equivalent calling setValue(index, NULL).
         * @param index
         */
        void unsetValue(int index);

        /**
         * @brief   Unsets all field values
         */
        void unsetValues();

        virtual QString getTypeName() const = 0;
        virtual QString getFieldName(int index) const = 0;
        virtual QVariant::Type getFieldType(int index) const = 0;

        /**
         * @brief   RowId of the current record
         * @return  Returns a positive value if
         */
        qlonglong id() const;
        void setId(qlonglong value);

    private:
        qlonglong _rowId;
        QList<QVariant*> _valueList;
};

class Table;

class Database : public QObject{
    Q_OBJECT

    public:
        Database(QString dbname, QSettings* settings, QObject* parent = NULL);

        QString getDBName() const;
        QString getDBPath() const;
        bool tableExists(QString tableName) const;

        /**
         * @brief	Constructs a Table object if none exists already for the
         *			requested table. If the table does not exist in the database
         *			it will be created, see Database::createTable() for details.
         *          If the table already exists its fields will be compared
         *          against those found in the example Datum. Any missing fields
         *          will result in returning NULL.
         *
         * @param   dataExample     An example of data stored in the requested table
         * @param   tableName		Name of the table to return a pointer to, if
         *                          empty string dataExample.getTypeName() will
         *                          be used instead.
         * @return	A pointer to the requested Table or NULL if an error was
         *			encountered.
         */
        Table* getTable(Datum* dataExample, QString tableName=QString());

        /**
         * @brief   If the table does not exist in the database it will be
         *          initialized with all fields from the supplied example Datum.
         *          The first field will be made the primary key. If the type of
         *          the first field is an INTEGER the primary key will be made
         *          auto-incrementing.
         *
         * @param   dataExample Field names and types will be used to create
         *                      table columns. Type name will be used as table
         *                      name unless overriden by supplying tableName.
         * @param   tableName   Used to name the created table, defaults to
         *                      empty string in which case dataExample.getTypeName()
         *                      will be used instead.
         * @return  Returns true if table could be created, false on errors
         */
        bool createTable(Datum* dataExample, QString tableName=QString());


        /**
         * @brief   Translates a QVariant::Type into a string suitable for use
         *          in an Sqlite database.
         * @param   variantType
         */
        static QString variantToSqlType(QVariant::Type variantType);

    signals:
        void databaseOpened(QString dbName, QString dbPath);

    protected:
        bool openDB();
        QString lookupDBPath(QSettings* settings);
        bool doQuery(QSqlQuery& query, QString queryString);

    private:
        QString _dbName;
        QString _dbconnectStr;
        QString _dbTypeStr;
        QSqlError _dbError;
        QSqlDatabase _db;
        QMap<QString, Table*> _tables;
};

class Table : public QObject{
        Q_OBJECT
    public:
        explicit Table(QString table, Datum* value, Database *parent);
        ~Table();

        /**
         * @brief	Verifies that the table exists in the database and has the
         *			same field names and types present in the example datum.
         * @return	Returns true if the table is found and has the expected
         *			fields.
         */
        bool isValid();


        template<typename DatumType> QList<Datum*> selectDataSync(QString query);

        bool insertDataSync(QList<Datum*> data, QList<int> fields=QList<int>());
        bool updateDataSync(QList<Datum*> data, QList<int> fields=QList<int>(), QList<int> matchOn=QList<int>());

        QString getTableName() const;
        QMap<QString,QVariant::Type> getColumnTypeMap();

    signals:
        void selectedData(QList<Datum*> data);

    public slots:
        //void selectData(QString query);
        void insertData(QList<Datum *> data, QList<int> fields);
        void updateData(QList<Datum*> data, QList<int> fields=QList<int>(), QList<int> matchOn=QList<int>());

    protected:
        bool doQuery(QSqlQuery& query, QString queryString);

    private:
        QString _tableName;
        QSqlDatabase _db;
        Database* _database;
        Datum* _exampleDatum;
};

#endif // DATABASE_H
