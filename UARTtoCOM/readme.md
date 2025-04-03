 Ngăn chặn thay đổi kích thước cửa sổ
 ```cpp
 case WM_NCHITTEST: 
 {
    LRESULT hit = DefWindowProc(hwnd, uMsg, wParam, lParam);
    if (hit == HTBOTTOMRIGHT || hit == HTTOPRIGHT || hit == HTBOTTOMLEFT || hit == HTTOPLEFT ||
        hit == HTBOTTOM || hit == HTRIGHT || hit == HTLEFT || hit == HTTOP) {
        return HTCLIENT; 
    }
    return 0;
 }
 ```

 Vẽ hình chữ nhật
 ```cpp
   HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255)); 
  SelectObject(hdc, hPen);
  Rectangle(hdc, 0, 0, 399, 299);
  DeleteObject(hPen);
 ```