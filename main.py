import sys

import pandas as pd
from rich import print
from rich.columns import Columns
from rich.console import Console
from rich.panel import Panel
from rich.table import Table
from rich.text import Text

NAMA_FILE_EXCEL = "data_tabel.xlsx"
NAMA_PROGRAM = "InterpKit"
VERSI_PROGRAM = "v1.1"

console = Console()


def tampilkan_header():
    console.clear()
    ascii_logo = f"""
[bold deep_sky_blue1]
/$$$$$$             /$$                                   /$$   /$$ /$$   /$$
|_  $$_/            | $$                                  | $$  /$$/|__/  | $$
 | $$   /$$$$$$$  /$$$$$$    /$$$$$$   /$$$$$$   /$$$$$$ | $$ /$$/  /$$ /$$$$$$
 | $$  | $$__  $$|_  $$_/   /$$__  $$ /$$__  $$ /$$__  $$| $$$$$/  | $$|_  $$_/
 | $$  | $$  \ $$  | $$    | $$$$$$$$| $$  \__/| $$  \ $$| $$  $$  | $$  | $$
 | $$  | $$  | $$  | $$ /$$| $$_____/| $$      | $$  | $$| $$\  $$ | $$  | $$ /$$
/$$$$$$| $$  | $$  |  $$$$/|  $$$$$$$| $$      | $$$$$$$/| $$ \  $$| $$  |  $$$$/
|______/|__/  |__/   \___/   \_______/|__/      | $$____/ |__/  \__/|__/   \___/
                                               | $$
                                               | $$
                                               |__/
[/bold deep_sky_blue1]
    """

    right_content = Text.from_markup(f"""
[bold]Selamat datang di InterpKit![/bold]

Silakan pilih menu:

 [cyan]1[/cyan]. Mulai Pencarian Data
 [cyan]2[/cyan]. Keluar
    """)

    layout_internal = Columns([ascii_logo, right_content], padding=1)

    print(
        Panel(
            layout_internal,
            title=f"[bold]{NAMA_PROGRAM}[/bold] [dim]{VERSI_PROGRAM}[/dim]",
            border_style="blue",
            padding=(1, 2),
        )
    )


def buat_tabel_hasil(data_series) -> Table:
    tabel = Table(show_header=False, border_style="dim", box=None, padding=(0, 1))
    tabel.add_column("Atribut", style="dim cyan", width=12)
    tabel.add_column("Nilai", style="bold")

    for index, value in data_series.items():
        if isinstance(value, float):
            value_str = f"{value:.4f}"
        else:
            value_str = str(value)

        tabel.add_row(str(index), value_str)
    return tabel


def cari_batas_data(df, search_column, input_value):
    try:
        df_sorted = df.sort_values(by=search_column)

        batas_bawah_rows = df_sorted[df_sorted[search_column] <= input_value]
        if batas_bawah_rows.empty:
            print(
                f"[bold red]Error: Nilai input {input_value} lebih kecil dari data terkecil.[/bold red]"
            )
            return None, None

        batas_bawah = batas_bawah_rows.iloc[-1]

        batas_atas_rows = df_sorted[df_sorted[search_column] >= input_value]
        if batas_atas_rows.empty:
            print(
                f"[bold red]Error: Nilai input {input_value} lebih besar dari data terbesar.[/bold red]"
            )
            return None, None

        batas_atas = batas_atas_rows.iloc[0]

        return batas_bawah, batas_atas

    except Exception as e:
        print(f"[bold red]Terjadi error saat mencari data: {e}[/bold red]")
        return None, None


def lakukan_interpolasi(available_columns, bawah, atas, x_col, x_target):
    try:
        print("\n[bold]Pilih kolom target untuk interpolasi:[/bold]")
        for i, col in enumerate(available_columns, start=1):
            if col == x_col:
                print(f" [dim]{i}. {col} (Kolom Basis)[/dim]")
            else:
                print(f" [cyan]{i}[/cyan]. {col}")
        print("---------------------------------")

        pilihan_str = input("Masukkan nomor kolom target: ")
        pilihan_int = int(pilihan_str)
        if not (1 <= pilihan_int <= len(available_columns)):
            raise IndexError("Pilihan di luar jangkauan")

        y_col = available_columns[pilihan_int - 1]
        if y_col == x_col:
            print(
                "[bold red]Error: Tidak bisa menginterpolasi kolom yang sama dengan basis.[/bold red]"
            )
            return

        x1 = float(bawah[x_col])
        x2 = float(atas[x_col])
        y1 = float(bawah[y_col])
        y2 = float(atas[y_col])
        x_target_float = float(x_target)

        y_target = y1 + ((x_target_float - x1) * (y2 - y1)) / (x2 - x1)

        hasil_text = Text(justify="center")
        hasil_text.append(f"Interpolasi untuk [bold]{y_col}[/bold]\n\n")
        hasil_text.append(f"X Target : {x_target_float}\n")
        hasil_text.append(f"Y Hasil  : [bold green]{y_target:.6f}[/bold green]\n\n")
        hasil_text.append(f"[dim]Berdasarkan:\n")
        hasil_text.append(f" (x1: {x1}, y1: {y1})\n")
        hasil_text.append(f" (x2: {x2}, y2: {y2})[/dim]")

        print(
            Panel(
                hasil_text,
                title="[bold green]HASIL INTERPOLASI LINIER[/bold green]",
                border_style="green",
            )
        )

    except (ValueError, TypeError):
        print(f"\n[bold red]Error: Kolom '{y_col}' berisi data non-numerik.[/bold red]")
        print("Interpolasi hanya bisa dilakukan pada angka.")
    except ZeroDivisionError:
        print(
            "\n[bold red]Error: Nilai batas atas dan bawah sama (pembagian dengan nol).[/bold red]"
        )
        print("Tidak dapat melakukan interpolasi.")
    except (IndexError, KeyError):
        print("\n[bold red]Error: Pilihan tidak valid.[/bold red]")
    except Exception as e:
        print(f"\n[bold red]Error saat interpolasi: {e}[/bold red]")


if __name__ == "__main__":
    try:
        df = pd.read_excel(NAMA_FILE_EXCEL)
        df.columns = df.columns.str.strip().str.replace("\u200b", "")
        available_columns = list(df.columns)

        tampilkan_header()

    except FileNotFoundError:
        print(f"[bold red]Error: File '{NAMA_FILE_EXCEL}' tidak ditemukan.[/bold red]")
        print("Pastikan file berada di folder yang sama dengan program.")
        sys.exit()
    except Exception as e:
        print(f"[bold red]Terjadi error saat memuat file: {e}[/bold red]")
        sys.exit()

    print("---------------------------------")
    pilihan_menu = input("Masukkan nomor pilihan: ")

    if pilihan_menu == "1":
        while True:
            try:
                print("\n[bold]Silakan pilih kolom basis pencarian:[/bold]")
                for i, col in enumerate(available_columns, start=1):
                    print(f" [cyan]{i}[/cyan]. {col}")
                print("---------------------------------")

                pilihan_str = input("Masukkan nomor pilihan (misal: 1): ")
                pilihan_int = int(pilihan_str)

                if not (1 <= pilihan_int <= len(available_columns)):
                    raise IndexError("Pilihan di luar jangkauan")

                kolom_pencarian = available_columns[pilihan_int - 1]
                print(f"Anda memilih: [bold green]{kolom_pencarian}[/bold green]")

                nilai_input_str = input(f"Masukkan nilai untuk {kolom_pencarian}: ")
                nilai_input = float(nilai_input_str)

            except (ValueError, IndexError):
                print(f"\n[bold red]Error: Input tidak valid.[/bold red] Coba lagi.\n")
                continue

            bawah, atas = cari_batas_data(df, kolom_pencarian, nilai_input)

            if bawah is not None and atas is not None:
                if bawah[kolom_pencarian] == atas[kolom_pencarian]:
                    tabel_hasil = buat_tabel_hasil(bawah)
                    print(
                        Panel(
                            tabel_hasil,
                            title="[bold green]Nilai Ditemukan Persis[/bold green]",
                            border_style="green",
                            padding=(1, 2),
                        )
                    )
                else:
                    tabel_bawah = buat_tabel_hasil(bawah)
                    tabel_atas = buat_tabel_hasil(atas)

                    print("\n[bold]Hasil Pencarian Batas Data:[/bold]")
                    hasil_layout = Columns(
                        [
                            Panel(
                                tabel_bawah,
                                title="[bold blue]BATAS BAWAH[/bold blue]",
                                subtitle=f"Nilai: [bold]{bawah[kolom_pencarian]}[/bold]",
                                border_style="blue",
                                padding=(1, 2),
                            ),
                            Panel(
                                tabel_atas,
                                title="[bold blue]BATAS ATAS[/bold blue]",
                                subtitle=f"Nilai: [bold]{atas[kolom_pencarian]}[/bold]",
                                border_style="blue",
                                padding=(1, 2),
                            ),
                        ]
                    )
                    print(hasil_layout)

                    print("\n" + "-" * 20)
                    pilihan_interp = input(
                        "[bold yellow]Apakah Anda ingin melakukan interpolasi? (y/n): [/bold yellow]"
                    )
                    if pilihan_interp.lower() == "y":
                        lakukan_interpolasi(
                            available_columns, bawah, atas, kolom_pencarian, nilai_input
                        )

            print("\n" + "=" * 40)
            pilihan_lanjut = input(
                "[bold yellow]Apakah Anda ingin melakukan pencarian lagi? (y/n): [/bold yellow]"
            )

            if pilihan_lanjut.lower() != "y":
                break

            print("\n")

    print("\n[dim]Terima kasih telah menggunakan InterpKit. Goodbye![/dim]")
    sys.exit()
