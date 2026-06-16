import sqlite3 as sq

cursor = sq.Cursor(sq.connect('web/olymp.db'))

cursor.execute("SELECT statement_html FROM tasks WHERE title='Graphforces'")
print(cursor.fetchone())