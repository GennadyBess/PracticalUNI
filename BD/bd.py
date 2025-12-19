import psycopg2
import sys
from PyQt5.QtWidgets import (
    QApplication, QLabel, QWidget, QMainWindow, QHBoxLayout, QTableWidget, QTableWidgetItem, 
    QPushButton, QHeaderView, QVBoxLayout, QComboBox, QGroupBox, QFrame, QLineEdit, QMessageBox,
    QInputDialog, QDialog
)
from PyQt5.QtCore import QTimer, Qt


DB_HOST = "127.0.0.1"
DB_NAME = "test_table"
DB_USER = "postgres"
DB_PASS = "8800"

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.conn = psycopg2.connect(host=DB_HOST, database=DB_NAME, user=DB_USER, password=DB_PASS)
        self.cur = self.conn.cursor()

        self.init_ui()
        self.load_data_from_db()

    def init_ui(self):
        self.setWindowTitle("База данных")
        self.setGeometry(100, 100, 1000, 600)

        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout()

        group_box = QGroupBox("")
        group_layout = QVBoxLayout()
        columns_layout = QHBoxLayout()
                
        separator = QFrame()
        separator.setFrameShape(QFrame.VLine) 
        separator.setFrameShadow(QFrame.Sunken)  
        separator.setLineWidth(1)  

        left_layout = QVBoxLayout()
        right_layout = QVBoxLayout()

        rows_data_right = [
        "building",
        "building_k",
        "floor",
        "telephone"
        ]
        self.inputs = {}
        self.tables = {}

        left_layout.addLayout(self.get_data_combobox("name_", "Имя"))
        left_layout.addLayout(self.get_data_combobox("last_name", "Фамилия"))
        left_layout.addLayout(self.get_data_combobox("middle_name", "Отчество"))
        left_layout.addLayout(self.get_data_combobox("street", "Улица"))

        for label_text in rows_data_right:
            row_layout, line = self.create_label_editline(label_text)
            self.inputs[label_text] = line
            right_layout.addLayout(row_layout)

        columns_layout.addLayout(left_layout)
        columns_layout.addWidget(separator)
        columns_layout.addLayout(right_layout)

        add_button = QPushButton("Добавить запись")
        add_button.clicked.connect(self.add_new_row)

        delete_button = QPushButton("Удалить строку")
        delete_button.clicked.connect(self.delete_selected_row)

        search_button = QPushButton("Поиск")
        search_button.clicked.connect(self.search)

        clear_search_button = QPushButton("Очистить поиск")
        clear_search_button.clicked.connect(self.clear_search)

        clear_form_button = QPushButton("Очистить форму")
        clear_form_button.clicked.connect(self.clear_form)

        buttons_layout1 = QHBoxLayout()
        buttons_layout1.addWidget(add_button)
        buttons_layout1.addWidget(delete_button)

        buttons_layout2 = QHBoxLayout()
        buttons_layout2.addWidget(search_button)
        buttons_layout2.addWidget(clear_search_button)
        buttons_layout2.addWidget(clear_form_button)

        group_layout.addLayout(columns_layout)
        group_layout.addLayout(buttons_layout1)  
        group_layout.addLayout(buttons_layout2)
        group_box.setLayout(group_layout)
        main_layout.addWidget(group_box)


        self.table_widget = QTableWidget()
        self.table_widget.setAlternatingRowColors(True)
        self.table_widget.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.table_widget.setSelectionBehavior(QTableWidget.SelectRows)

        main_layout.addWidget(self.table_widget)
        central_widget.setLayout(main_layout)

    def get_data_combobox(self, table_name, label_name):
        self.cur.execute(f"SELECT * FROM {table_name}")
        items = self.cur.fetchall()
        self.tables[table_name] = items
        layout, combo = self.create_label_combobox(f"{label_name}:", items)

        manage_btn = QPushButton("⚙")  
        manage_btn.setFixedSize(25, 25)
        manage_btn.clicked.connect(lambda: self.open_table_manager(table_name))
        layout.addWidget(manage_btn)

        self.inputs[f"{table_name}"] = combo
        return layout

    def create_label_combobox(self, label_text, items):
        layout = QHBoxLayout()
        label = QLabel(label_text)
        label.setFixedWidth(100)
        values = []
        for _, value in items:
            values.append(value)

        box = QComboBox()
        box.addItem("")
        box.addItems(values)
        box.setMinimumWidth(200)

        layout.addWidget(label)
        layout.addWidget(box)
        layout.addStretch()
        return layout, box
    
    def create_label_editline(self, label_text):
        layout = QHBoxLayout()
        label = QLabel(label_text)
        label.setFixedWidth(100)

        line = QLineEdit()
        line.setMinimumWidth(200)

        layout.addWidget(label)
        layout.addWidget(line)
        layout.addStretch()
        return layout, line
    
    def load_data_from_db(self):
        table = self.process_cmd("""SELECT 
            m.unique_id,
            n.name_val, 
            l.last_name_val,  
            mn.middle_name_val, 
            s.street_val, 
            m.building,
            m.building_k,
            m.floor,
            m.telephone
            FROM main m
            LEFT JOIN name_ n ON m.name_ = n.id
            LEFT JOIN last_name l ON m.last_name = l.id
            LEFT JOIN middle_name mn ON m.middle_name = mn.id
            LEFT JOIN street s ON m.street = s.id
            ORDER BY m.unique_id ASC;
                                """)
        self.table_widget.setRowCount(len(table))
        self.table_widget.setColumnCount(len(table[0]) + 1)
        self.table_widget.setSelectionBehavior(QTableWidget.SelectRows)

        headers = ["id", "name_", "last_name", "middle_name", "street", "building", "building_k", "floor", "telephone", "action"]
        self.table_widget.setHorizontalHeaderLabels(headers)

        for row_num, row_data in enumerate(table):
            for col_num, cell_data in enumerate(row_data):
                item = QTableWidgetItem(str(cell_data) if cell_data is not None else "NULL")
                self.table_widget.setItem(row_num, col_num, item)
            self.add_update_button(row_num)

    def add_update_button(self, row):
        widget = QWidget()
        
        layout = QHBoxLayout()
        layout.setContentsMargins(2, 2, 2, 2)  
        update_btn = QPushButton("Обновить") 
        update_btn.clicked.connect(lambda checked, r=row: self.update_row(r))
        layout.addWidget(update_btn)
        layout.addStretch()
        widget.setLayout(layout)
        self.table_widget.setCellWidget(row, self.table_widget.columnCount() - 1, widget)
    
    def update_row(self, row):
        updated_data = {}
        for col in range(self.table_widget.columnCount() - 1):
            header = self.table_widget.horizontalHeaderItem(col).text()
            item = self.table_widget.item(row, col)
            updated_data[header] = item.text()
        updated_data["id"] = int(updated_data["id"])

        self.converting("name_", updated_data)
        self.converting("last_name", updated_data)
        self.converting("middle_name", updated_data)
        self.converting("street", updated_data)

        items = []
        for header, value in updated_data.items():
            items.append(f"{header} = {value}")

        update_query = f"""
                UPDATE main 
                SET {', '.join(items[1:])}
                WHERE unique_id = {updated_data["id"]}
            """
        try:
            rows_updated = self.process_cmd(update_query)
        except Exception:
            self.show_warning(self, "Строка НЕ обновлена,\n Введите корректные значения!")
            return

        if rows_updated:
            self.load_data_from_db()
            self.show_warning(self, "Строка обновлена")
        else:
            self.show_warning(self, "Не удалось обновить")

    def search(self):
        values = {}
        for key, item in self.inputs.items():
            if isinstance(item, QComboBox):
                values[key] = item.currentText()
            elif isinstance(item, QLineEdit):
                try:
                    values[key] = int(item.text())
                except ValueError:
                    values[key] = item.text()

        self.converting("name_", values)
        self.converting("last_name", values)
        self.converting("middle_name", values)
        self.converting("street", values)

        where_conditions = []
        for field, value in values.items():
            if value: 
                where_conditions.append(f"m.{field} = '{value}'")
        
        if not(where_conditions):
            self.show_warning(self, "Введите значения для поиска")
            return
        
        print(where_conditions)
        query = f"""
            SELECT 
            m.unique_id,
            n.name_val,
            l.last_name_val,
            mn.middle_name_val,
            s.street_val,
            m.building,
            m.building_k,
            m.floor,
            m.telephone
        FROM main m
        LEFT JOIN name_ n ON m.name_ = n.id
        LEFT JOIN last_name l ON m.last_name = l.id
        LEFT JOIN middle_name mn ON m.middle_name = mn.id
        LEFT JOIN street s ON m.street = s.id
        WHERE {" AND ".join(where_conditions)}
        ORDER BY m.unique_id ASC
        """

        table = self.process_cmd(query)
        if not(table):
            self.show_warning(self, "Ничего не найдено")
            return

        self.table_widget.setRowCount(len(table))
        self.table_widget.setColumnCount(len(table[0]) + 1)
        self.table_widget.setSelectionBehavior(QTableWidget.SelectRows)

        headers = ["id", "name_", "last_name", "middle_name", "street", "building", "building_k", "floor", "telephone", "action"]
        self.table_widget.setHorizontalHeaderLabels(headers)

        for row_num, row_data in enumerate(table):
            for col_num, cell_data in enumerate(row_data):
                item = QTableWidgetItem(str(cell_data) if cell_data is not None else "NULL")
                self.table_widget.setItem(row_num, col_num, item)
            self.add_update_button(row_num)

    def clear_search(self):
        self.load_data_from_db()
        self.show_warning(self, "Показаны все записи")

    def clear_form(self):
        for item in self.inputs.values():
            if isinstance(item, QComboBox):
                item.setCurrentIndex(0)
            elif isinstance(item, QLineEdit):
                item.clear()
        self.show_warning(self, "Форма очищена")

    def open_table_manager(self, table_name):
        dialog = TableManagerDialog(self, table_name, self.conn)
        dialog.exec_()
        self.reload_combobox(table_name)

    def reload_combobox(self, table_name):
            self.cur.execute(f"SELECT * FROM {table_name}")
            items = self.cur.fetchall()
            self.tables[table_name] = items
            
            combo = self.inputs[table_name]
            combo.clear()
            combo.addItem("")
            for _, value in items:
                combo.addItem(value)

    def add_new_row(self):
        values = {}
        for key, item in self.inputs.items():
            if isinstance(item, QComboBox):
                values[key] = item.currentText()
            elif isinstance(item, QLineEdit):
                try:
                    values[key] = int(item.text())
                except ValueError:
                    values[key] = item.text()
        
        if any(v == "" for v in values.values()):
            self.show_warning(self, "Заполните все поля!")
            return

        self.converting("name_", values)
        self.converting("last_name", values)
        self.converting("middle_name", values)
        self.converting("street", values)
        
        query = """
                INSERT INTO main (name_, last_name, middle_name, street, building, building_k, floor, telephone) VALUES (%s, %s, %s, %s, %s, %s, %s, %s)
            """

        self.cur.execute(query, tuple(values.values()))
        self.conn.commit()
        self.load_data_from_db() 
    
    def delete_selected_row(self): 
        current_row = self.table_widget.currentRow()
        if current_row == -1:
            self.show_warning(self, "Выберите строку для удаления!")
            return

        item = self.table_widget.item(current_row, 0)
        
        result = self.process_cmd(f"DELETE FROM main WHERE unique_id = {item.text()}")
            
        if result:
            self.table_widget.removeRow(current_row)
        else:
            self.show_warning(self, "Не удалось удалить запись из базы данных")

    def converting(self, table_name, values):
        for el in self.tables[table_name]:
            if values[table_name] == el[1]:
                values[table_name] = el[0]

    def show_warning(self, parent, text):
        label = QLabel(text, parent)
        label.setStyleSheet("""
            background: rgba(0, 0, 0, 200);
            color: white;
        """)
        label.setAlignment(Qt.AlignCenter)
        label.resize(300, 50)
        w = parent.width()
        h = parent.height()
        label.move(w//2 - 150, h//2 - 25)
        label.show()

        QTimer.singleShot(2000, label.close)

    def process_cmd(self, cmd):
        if cmd.strip().upper().startswith('SELECT'):
            self.cur.execute(cmd)
            self.conn.commit()
            return self.cur.fetchall()
        else:
            self.cur.execute(cmd)
            self.conn.commit()
            rows_affected = self.cur.rowcount
            return rows_affected

    def closeEvent(self, event):
        self.cur.close()
        self.conn.close()
        event.accept()

class TableManagerDialog(QDialog):
    def __init__(self, parent, table_name, conn):
        super().__init__(parent)
        self.table_name = table_name
        self.conn = conn
        self.cur = conn.cursor()
        
        self.init_ui()
        self.load_data_from_db()

    def init_ui(self):
        self.setWindowTitle(f"Управление таблицей {self.table_name}")
        self.setGeometry(300, 300, 500, 400)

        layout = QVBoxLayout()
        
        input_layout = QHBoxLayout()
        self.input_field = QLineEdit()
        self.input_field.setPlaceholderText("Новое значение...")
        add_button = QPushButton("Добавить")
        add_button.clicked.connect(self.add_row)
        input_layout.addWidget(QLabel("Значение:"))
        input_layout.addWidget(self.input_field)
        input_layout.addWidget(add_button)
        
        button_layout = QHBoxLayout()

        delete_button = QPushButton("Удалить")
        delete_button.clicked.connect(self.delete_selected_row)

        button_layout.addWidget(delete_button)
        button_layout.addStretch()

        self.table = QTableWidget()
        self.table.setColumnCount(3)
        self.table.setHorizontalHeaderLabels(["ID", "Значение", "Действие"])
        self.table.horizontalHeader().setSectionResizeMode(1, 1)  
        self.table.setSelectionBehavior(1)  
        
        layout.addLayout(input_layout)
        layout.addWidget(self.table)
        layout.addLayout(button_layout)
        self.setLayout(layout)

    def load_data_from_db(self):
        self.cur.execute(f"SELECT * FROM {self.table_name} ORDER BY id")
        rows = self.cur.fetchall()
        
        self.table.setRowCount(len(rows))
        for row_num, (id_val, value) in enumerate(rows):
            self.table.setItem(row_num, 0, QTableWidgetItem(str(id_val)))
            self.table.setItem(row_num, 1, QTableWidgetItem(value))
            self.add_update_button(row_num) 

    def add_row(self):
        value = self.input_field.text().strip()
        if not value:
            self.show_warning(self, f"Введите значение")
            return
        try:  
            self.cur.execute(f"INSERT INTO {self.table_name} ({(self.table_name+"_val").replace("__", "_")}) VALUES (%s)", (value,))
            self.conn.commit()
            self.input_field.clear()
            self.load_data_from_db()
        except Exception:
            print(Exception)
            self.show_warning(self, f"Ошибка добавления")

    def update_row(self):
        current_row = self.table.currentRow()
        if current_row < 0:
            self.show_warning(self, "Выберете строку!")
            return
        id_val = self.table.item(current_row, 0).text()
        value = self.table.item(current_row, 1).text()
        self.cur.execute(f"SELECT {self.table_name} FROM main")
        values = [row[0] for row in self.cur.fetchall()]
        if int(id_val) in values:
            self.show_warning(self, "Нельзя изменить \n значение используется в основной таблице ")
            return
        try:
            self.cur.execute(f"UPDATE {self.table_name} SET {(self.table_name+"_val").replace("__", "_")} = %s WHERE id = %s", (value, id_val))
            self.conn.commit()
            self.load_data_from_db()
        except Exception :
            self.show_warning(self, f"Ошибка изменения")
            return
        self.show_warning(self, "Строка обновлена")

    def add_update_button(self, row):
        widget = QWidget()
        
        layout = QHBoxLayout()
        layout.setContentsMargins(2, 2, 2, 2)  
        update_btn = QPushButton("Обновить") 
        update_btn.clicked.connect(self.update_row)
        layout.addWidget(update_btn)
        layout.addStretch()
        widget.setLayout(layout)
        self.table.setCellWidget(row, self.table.columnCount() - 1, widget)

    def delete_selected_row(self):
        current_row = self.table.currentRow()
        if current_row < 0:
            self.show_warning(self, "Выбрете строчку!")
            return
        
        id_val = self.table.item(current_row, 0).text()
        self.cur.execute(f"SELECT {self.table_name} FROM main")
        values = [row[0] for row in self.cur.fetchall()]
        if int(id_val) in values:
            self.show_warning(self, "Нельзя удалить\n значение используется в основной таблице ")
            return
        try:
            self.cur.execute(f"DELETE FROM {self.table_name} WHERE id = %s", (id_val,))
            self.conn.commit()
            self.load_data_from_db()
        except Exception:
            self.show_warning(self, "Ошибка удаления!")
        
    def show_warning(self, parent, text):
        label = QLabel(text, parent)
        label.setStyleSheet("""
            background: rgba(0, 0, 0, 200);
            color: white;
        """)
        label.setAlignment(Qt.AlignCenter)
        label.resize(300, 50)
        w = parent.width()
        h = parent.height()
        label.move(w//2 - 150, h//2 - 25)
        label.show()

        QTimer.singleShot(2000, label.close)
    
def main():
    app = QApplication(sys.argv)

    window = MainWindow()
    window.show()

    sys.exit(app.exec_())

main()
