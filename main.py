import sys

from rich import print

import config
import logic
import ui


def main():
    df, available_columns = logic.load_and_clean_data(config.NAMA_FILE_EXCEL)
    ui.tampilkan_header()

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

            bawah, atas = logic.cari_batas_data(df, kolom_pencarian, nilai_input)

            if bawah is not None and atas is not None:
                ui.tampilkan_hasil_pencarian(kolom_pencarian, bawah, atas)

                if bawah[kolom_pencarian] != atas[kolom_pencarian]:
                    print("\n" + "-" * 20)
                    pilihan_interp = input(
                        "[bold yellow]Apakah Anda ingin melakukan interpolasi? (y/n): [/bold yellow]"
                    )

                    if pilihan_interp.lower() == "y":
                        try:
                            print(
                                "\n[bold]Pilih kolom target untuk interpolasi:[/bold]"
                            )
                            for i, col in enumerate(available_columns, start=1):
                                if col == kolom_pencarian:
                                    print(f" [dim]{i}. {col} (Kolom Basis)[/dim]")
                                else:
                                    print(f" [cyan]{i}[/cyan]. {col}")
                            print("---------------------------------")

                            pilihan_y_str = input("Masukkan nomor kolom target: ")
                            pilihan_y_int = int(pilihan_y_str)
                            if not (1 <= pilihan_y_int <= len(available_columns)):
                                raise IndexError("Pilihan di luar jangkauan")

                            y_col = available_columns[pilihan_y_int - 1]
                            if y_col == kolom_pencarian:
                                print(
                                    "[bold red]Error: Tidak bisa menginterpolasi kolom yang sama dengan basis.[/bold red]"
                                )
                                continue

                            x1 = float(bawah[kolom_pencarian])
                            x2 = float(atas[kolom_pencarian])
                            y1 = float(bawah[y_col])
                            y2 = float(atas[y_col])

                            y_target, error = logic.hitung_interpolasi(
                                x1, x2, y1, y2, nilai_input
                            )

                            if error:
                                print(f"[bold red]{error}[/bold red]")
                            else:
                                ui.tampilkan_hasil_interpolasi(
                                    y_col, y_target, nilai_input, x1, x2, y1, y2
                                )

                        except (ValueError, TypeError):
                            print(
                                f"\n[bold red]Error: Kolom '{y_col}' berisi data non-numerik.[/bold red]"
                            )
                        except (IndexError, KeyError):
                            print("\n[bold red]Error: Pilihan tidak valid.[/bold red]")

            print("\n" + "=" * 40)
            pilihan_lanjut = input(
                "[bold yellow]Apakah Anda ingin melakukan pencarian lagi? (y/n): [/bold yellow]"
            )
            if pilihan_lanjut.lower() != "y":
                break
            print("\n")

    print("\n[dim]Terima kasih telah menggunakan InterpKit. Goodbye![/dim]")
    sys.exit()


if __name__ == "__main__":
    main()
