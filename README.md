# InterpKit v1.1

`InterpKit` adalah sebuah alat bantu CLI (Command Line Interface) yang dirancang untuk mempercepat dan mempermudah pencarian data dari tabel rekayasa, khususnya tabel fungsi `d/L` untuk pertambahan nilai `d/L0`.

Program ini dibuat untuk mengatasi proses manual yang lambat dan rawan kesalahan dalam mencari nilai batas atas dan batas bawah dari tabel, yang kemudian digunakan untuk perhitungan interpolasi linier.

-----

## Fitur Utama

  * **Tampilan CLI Modern:** Antarmuka interaktif yang bersih dan fungsional, ditenagai oleh *library* `rich`.
  * **Pencarian Cepat:** Secara instan menemukan baris data batas atas dan batas bawah dari nilai input Anda.
  * **Kalkulator Interpolasi:** Otomatis menghitung interpolasi linier setelah data batas ditemukan.
  * **Modular:** Kode dipecah menjadi file `logic.py`, `ui.py`, dan `config.py` agar mudah dikelola dan dikembangkan.
  * **Distribusi Mudah:** Dapat dibundel menjadi satu file `.exe` mandiri menggunakan PyInstaller, yang sudah menyertakan data Excel di dalamnya.

-----

## Instalasi & Persiapan

Ada dua cara untuk menjalankan program ini, tergantung kebutuhan Anda.

### Metode 1: Untuk Pengguna (Rilis `.exe`)

Cara termudah untuk menjalankan program tanpa perlu menginstal Python.

**Via `winget` (Jika Sudah Didaftarkan):**

```bash
winget install NamaAnda.InterpKit
```

*(Ganti `NamaAnda.InterpKit` dengan ID Paket Anda yang sebenarnya)*

**Via GitHub Releases (Unduh Manual):**

1.  Buka halaman **Releases** di repositori ini.
2.  Unduh file `InterpKit.exe` (versi *embedded*) atau file `.zip`.
3.  Jalankan file `.exe` tersebut.

### Metode 2: Untuk Developer (Menjalankan dari Source)

Jika Anda ingin memodifikasi atau menjalankan kode sumbernya langsung.

1.  **Clone Repositori:**

    ```bash
    git clone https://github.com/USERNAMEANDA/REPOANDA.git
    cd InterpKit
    ```

2.  **Buat Virtual Environment (Opsional tapi direkomendasikan):**

    ```bash
    python -m venv venv
    .\venv\Scripts\activate
    ```

3.  **Instal Kebutuhan:**
    Proyek ini membutuhkan `pandas`, `rich`, dan `openpyxl`.

    ```bash
    pip install pandas rich openpyxl
    ```

    (Atau, jika ada `requirements.txt`: `pip install -r requirements.txt`)

-----

## Cara Penggunaan

1.  Pastikan file `data_tabel.xlsx` berada di folder yang sama dengan `main.py` (jika menjalankan dari *source*) atau sudah dibundel (jika menggunakan `.exe`).
2.  Jalankan program:
    ```bash
    python main.py
    ```

### Alur Program:

1.  Anda akan disambut oleh *splash screen* `InterpKit`.
2.  Pilih `1` untuk **Mulai Pencarian Data**.
3.  Pilih nomor kolom yang akan dijadikan **basis pencarian** (misal: `1` untuk `d/L0`).
4.  Masukkan **nilai** yang ingin Anda cari (misal: `0.0307`).
5.  Program akan menampilkan data batas atas (`0.0310`) dan batas bawah (`0.0300`) dalam tabel yang rapi.
6.  Pilih `y` untuk **melakukan interpolasi**.
7.  Pilih nomor kolom yang akan dijadikan **target interpolasi** (misal: `8` untuk `K`).
8.  Program akan menghitung dan menampilkan nilai `K` yang terinterpolasi (`0.0898`).

-----

## Struktur Proyek

Proyek ini dibagi menjadi beberapa modul untuk mempraktikkan *Separation of Concerns*:

  * **`main.py`**: Titik masuk utama. Bertindak sebagai *controller* yang mengatur alur program.
  * **`logic.py`**: Berisi semua "otak" program. Menangani pemuatan data `pandas`, pencarian batas, dan kalkulasi interpolasi.
  * **`ui.py`**: Mengelola semua *output* ke terminal. Menangani semua fungsi `rich` untuk mencetak *header*, *panel*, dan *tabel*.
  * **`config.py`**: Menyimpan konstanta dan konfigurasi global (seperti nama program dan nama file Excel).

-----

## Lisensi

Proyek ini dilisensikan di bawah Lisensi MIT. Lihat file `LICENSE` untuk detailnya.
