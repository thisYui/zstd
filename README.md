# MyProject - zstd 
Dự án này không thuộc vào chương trình học phần nào, mà là dự án của cá nhân tôi.

## Description
- Đây là một dự án nén dữ liệu mô phỏng zstd bằng cách sử dụng LZ77 và mã hóa Huffman. 
- Dụ án này là dự án cá nhân được thực hiện bởi Nguyễn Quang Duy, sinh viên Trường Đại học Khoa học Tự nhiên, ĐHQG-TPHCM.

## Features
- Feature 1: Dụ án có thể thực hiện phéo nén LZ77 và mã hóa Huffman cho dữ liệu đầu vào.
- Feature 2: Giao diện dòng lệnh.
- Feature 3: Hỗ trợ nén và giải nén dữ liệu đối với folder/file thành một file .bin.

## Requirements
- Operating System: Linux / macOS / Windows
- Programming Language: C++17 hoặc cao hơn.
- Dependencies: Được code bằng Clion.

## Attention
- Thuật toán LZ77 có tốc độ nén không tốt cho dữ liệu ngẫu nhiên, nhưng tốc độ nén tốt cho dữ liệu lặp lại nhiều lần. Ngoài ra thuật toán chạy với tốc độ chậm có thể lên tới O(n^2) với n là kích thước của dữ liệu đầu vào.
- Thuật toán Huffman sở hữu tốc độ cao hơn nhiều so với LZ77, người đọc có thể tham khảo kết hợp Huffman với nhiều loại thuật toán nén khác nhau không chỉ riêng với LZ77.
- Thuật toán sẽ có thời gian chờ lâu với dữ liệu trên 1MB do sử dụng LZ77.

## Operating
- Bước 1: Clone dự án về máy của bạn.
- Bước 2: Chạy dự án bằng Clion hoặc các IDE/Text Editor khác. Chú ý: Đảm bảo rằng bạn đã cài đặt CMake và CMakeList.txt đã được tạo.
- Bước 3: Có thể nén/giải nén dữ liệu, lưu ý cung cấp đuường dẫn cụ thể cho dữ liệu đầu vào. Ví dụ: "D:/source".
