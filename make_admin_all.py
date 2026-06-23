import sqlite3 as sq

cursor = sq.Cursor(sq.connect('web/olymp.db'))

cursor.execute("UPDATE users SET is_admin=1")
cursor.connection.commit()
cursor.close()