from rich import print
from rich.columns import Columns
from rich.console import Console
from rich.panel import Panel
from rich.table import Table
from rich.text import Text

from config import NAMA_PROGRAM, VERSI_PROGRAM

console = Console()


def tampilkan_header():
    console.clear()
    ascii_logo = f"""
[bold deep_sky_blue1]
/$$$$$$             /$$                                   /$$   /$$ /$$   /$$
|_ $$_/            | $$                                  | $$  /$$/ |__/ | $$
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


def tampilkan_hasil_pencarian(kolom_pencarian, bawah, atas):
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


def tampilkan_hasil_interpolasi(y_col, y_target, x_target, x1, x2, y1, y2):
    markup_string = f"""
Interpolasi untuk [bold]{y_col}[/bold]

X Target : {x_target}
Y Hasil  : [bold green]{y_target:.6f}[/bold green]

[dim]Berdasarkan:
 (x1: {x1}, y1: {y1})
 (x2: {x2}, y2: {y2})[/dim]
"""
    hasil_text = Text.from_markup(markup_string, justify="center")

    print(
        Panel(
            hasil_text,
            title="[bold green]HASIL INTERPOLASI LINIER[/bold green]",
            border_style="green",
        )
    )
