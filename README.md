Zstd_Project/
│
├── src/                      # Mã nguồn chính của dự án
│   ├── core/                 # Các thuật toán nén chính (LZ77, Huffman, FSE)
│   │   ├── lz77.cpp          # Cài đặt thuật toán LZ77
│   │   ├── huffman.cpp       # Cài đặt thuật toán Huffman
│   │   └── fse.cpp           # Cài đặt thuật toán FSE (Entropy Coding)
│   │
│   ├── io/                   # Xử lý I/O (đọc/ghi tệp tin nén và giải nén)
│   │   ├── file_io.cpp       # Các thao tác đọc/ghi tệp tin nén
│   │   └── buffer.cpp        # Quản lý bộ đệm dữ liệu
│   │
│   ├── utils/                # Các tiện ích hỗ trợ khác (tạo mã, kiểm tra đầu vào, v.v.)
│   │   ├── checksum.cpp      # Hàm tính checksum để kiểm tra tính toàn vẹn của dữ liệu
│   │   └── bitstream.cpp     # Các thao tác về bitstream (mã hóa và giải mã bit)
│   │
│   ├── zstd_compressor.cpp   # Cài đặt chức năng nén chính của Zstd
│   └── zstd_decompressor.cpp # Cài đặt chức năng giải nén chính của Zstd
│
├── include/                  # Thư mục chứa các tệp tiêu đề (header files)
│   ├── lz77.h                # Định nghĩa các hàm và cấu trúc dữ liệu cho LZ77
│   ├── huffman.h             # Định nghĩa các hàm và cấu trúc dữ liệu cho Huffman
│   ├── fse.h                 # Định nghĩa các hàm và cấu trúc dữ liệu cho FSE
│   ├── file_io.h             # Định nghĩa các hàm xử lý I/O tệp tin
│   ├── buffer.h              # Định nghĩa cấu trúc dữ liệu bộ đệm
│   ├── checksum.h            # Định nghĩa các hàm tính checksum
│   ├── bitstream.h           # Định nghĩa các thao tác bitstream
│   └── zstd.h                # Định nghĩa các hàm nén và giải nén của Zstd
│
├── test/                     # Thư mục chứa các bài kiểm tra
│   ├── test_lz77.cpp         # Kiểm tra LZ77
│   ├── test_huffman.cpp      # Kiểm tra Huffman
│   ├── test_fse.cpp          # Kiểm tra FSE
│   ├── test_compressor.cpp   # Kiểm tra nén Zstd
│   └── test_decompressor.cpp # Kiểm tra giải nén Zstd
│
├── docs/                     # Tài liệu dự án (hướng dẫn sử dụng, tài liệu kỹ thuật)
│   ├── README.md             # Tài liệu hướng dẫn sử dụng dự án
│   ├── design.md             # Tài liệu thiết kế thuật toán
│   └── performance.md        # Tài liệu tối ưu hóa và hiệu suất
│
├── examples/                 # Ví dụ minh họa sử dụng Zstd
│   └── example_compress.cpp  # Ví dụ nén dữ liệu với Zstd
│
├── CMakeLists.txt            # Cấu hình build cho CMake (hoặc Makefile nếu dùng Make)
└── LICENSE                   # Giấy phép dự án

[Dữ liệu gốc] → [Chia block] → [LZ77 nén] → [Huffman/FSE mã hóa] → [Ghi file]
                     ↑                                   ↓
               [Giải nén LZ77] ← [Giải mã Huffman/FSE] ← [Đọc file]
