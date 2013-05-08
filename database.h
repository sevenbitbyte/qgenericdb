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

		//void loadValues(QList<QVariant&> valueMap);
		QList<QVariant*> getValues() const;
		QList<QString> getFieldNames() const;

        /**
         * @brief Assigns the supplied value to the specified index
         * @param index
         * @param value
         */
        void setValue(int index, QVariant& value);
        QVariant* getValue(int index) const;


        virtual QString getTypeName() const = 0;
        virtual QString getFieldName(int index) const = 0;
        virtual QVariant::Type getFieldType(int index) const = 0;


        qlonglong id();

	protected:
		void initializeField(QVariant* value, int pos);

	private:
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
		 *			it will be initialized with all fields from the supplied
         *			example Datum. If a field named "id" exists it will be made
		 *			the primary key. If the table already exists its fields will
		 *			be compared against those found in the example Datum. Any
		 *			missing fields will result in returning NULL and emitting an
		 *			appropriate TableError.
         * @param   tableName		Name of the table to return a pointer to
         * @param   dataExample     An example of data stored in the requested table
		 * @return	A pointer to the requested Table or NULL if an error was
		 *			encountered.
		 */
		Table* getTable(QString tableName, Datum* dataExample);
		bool createTable();

		enum TableError {None=0x0, Extra_Table_Fields=1, Missing_Datum_Fields=2};

	signals:
		void tableError(QString tableName, TableError errorCode);
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
		QString _host;
		QSqlDatabase _db;
		QMap<QString, Table*> _tables;
};

class Table : public QObject{
		Q_OBJECT
	public:
		explicit Table(QString table, Datum* value, Database *parent);
		~Table();

		/**
		 * @brief	Varifies that the table exists in the database and has the
		 *			same field names and types present in the example datum.
		 * @return	Returns true if the table is found and has the expected
		 *			fields.
		 */
		bool isValid();


		struct Filter{
            enum ComparisonType{ Nop=0, Equal_To, Not_Equal, Greater_Than, Less_Than, Greater_Than_Eq, Less_Than_Eq};

			ComparisonType startCompare;
			ComparisonType endCompare;
			Datum* start;
			Datum* end;
		};

		QList<Datum*> selectData( QList<Filter> filters=QList<Filter>(), QString fieldList=QString("*") );
		QList<Datum*> selectData( QList<Filter> filters=QList<Filter>(), QList<int> requestedFields=QList<int>() );

		QString getTableName() const;
        QMap<QString,QVariant::Type> getColumnTypeMap();

	signals:
		void selectedData(QList<Datum*> data);

	public slots:
		void insertData(QList<Datum*> data, QList<int> fields=QList<int>());
		void updateData(QList<Datum*> data, QList<int> fields=QList<int>());

	protected:
		bool doQuery(QSqlQuery& query, QString queryString);

	private:
		QString _tableName;
		QList<Filter> _filters;
		QSqlDatabase _db;
		Database* _database;
		Datum* _exampleDatum;
};

#endif // DATABASE_H
