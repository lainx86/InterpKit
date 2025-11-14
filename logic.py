import sys
import os
import pandas as pd
from rich import print

def resource_path(relative_path):
    try:
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)


def load_and_clean_data(file_path):
    
    excel_path = resource_path(file_path)

    try:
        df = pd.read_excel(excel_path)
        df.columns = df.columns.str.strip().str.replace("\u200b", "")
        available_columns = list(df.columns)
        return df, available_columns
    except FileNotFoundError:
        print(f"[bold red]Error: File data '{file_path}' tidak ditemukan.[/bold red]")
        print("Pastikan file sudah dibundel dengan benar oleh PyInstaller.")
        sys.exit()
    except Exception as e:
        print(f"[bold red]Terjadi error saat memuat file: {e}[/bold red]")
        sys.exit()


def cari_batas_data(df, search_column, input_value):
    try:
        df_sorted = df.sort_values(by=search_column)
        
        batas_bawah_rows = df_sorted[df_sorted[search_column] <= input_value]
        if batas_bawah_rows.empty:
            print(f"[bold red]Error: Nilai input {input_value} lebih kecil dari data terkecil.[/bold red]")
            return None, None

        batas_bawah = batas_bawah_rows.iloc[-1]
        
        batas_atas_rows = df_sorted[df_sorted[search_column] >= input_value]
        if batas_atas_rows.empty:
            print(f"[bold red]Error: Nilai input {input_value} lebih besar dari data terbesar.[/bold red]")
            return None, None

        batas_atas = batas_atas_rows.iloc[0]
        
        return batas_bawah, batas_atas

    except Exception as e:
        print(f"[bold red]Terjadi error saat mencari data: {e}[/bold red]")
        return None, None


def hitung_interpolasi(x1, x2, y1, y2, x_target):
    try:
        y_target = y1 + ((x_target - x1) * (y2 - y1)) / (x2 - x1)
        return y_target, None
    except ZeroDivisionError:
        return None, "Error: Nilai batas atas dan bawah sama (pembagian dengan nol)."
    except Exception as e:
        return None, f"Error saat kalkulasi: {e}"