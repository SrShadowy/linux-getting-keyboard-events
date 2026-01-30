#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <unistd.h>

///////////////////////////////////
//// é bem o nome ja descreve   ///
//// tudo que é... n retorna    ///
///  nada.                      ///
///////////////////////////////////
#define MaxDevicesReader 32
void get_device_names(const char *dev = "/dev/input/event") {
  for (int i = 0; i < MaxDevicesReader; i++) {
    std::string path = dev + std::to_string(i);
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
      std::cout << "Cannot open try open with sudo\n";
      continue;
    }

    char name[256] = "Unknown";
    if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0) {
      std::cout << path << " -> " << name << std::endl;
    }
    close(fd);
  }
}

////////////////////////////////////
///  recebe o codigo do input    ///
//   e se for  retorna           ///
//   o caracter que representa,  ///
//   caso não, retorna -1        ///
////////////////////////////////////
char decode_key(__u16 code) {

  // numeros :)
  if (code >= KEY_1 && code <= KEY_9)
    return '1' + (code - KEY_1);
  if (code == KEY_0)
    return '0';

  

    // linhas do teclado 
  static const struct {
    __u16 start, end;
    const char *chars;
  } rows[] = {{KEY_Q, KEY_P, "QWERTYUIOP"},
              {KEY_A, KEY_L, "ASDFGHJKL"},
              {KEY_Z, KEY_M, "ZXCVBNM"}};

  for (const auto &row : rows) {
    if (code >= row.start && code <= row.end)
      return row.chars[code - row.start];
  }


  //caracteres especiaais 
  switch (code) {
  case KEY_TAB:
    return '\t';
  case KEY_ENTER:
    return '\n';
  case KEY_DOT:
    return '.';
  case KEY_SPACE:
    return ' ';
  case KEY_COMMA:
    return ',';
  case KEY_MINUS:
    return '-';
  case KEY_SLASH:
    return '/';
  }

  return -1;
}

int main() {

  std::cout.setf(std::ios::unitbuf); // sempre da flash no cout

  get_device_names();

  input_event device_input;

  int device_number = 0;

  std::cout << "Select input event number: ";
  std::cin >> device_number;

  std::string path = "/dev/input/event" + std::to_string(device_number);

  std::ifstream devices(path, std::ios::binary);
  if (!devices.is_open()) {
    std::cout << "Cannot open the file";
    return 1;
  }
  std::cout << "Okey loop initializing\n";

  while (true) {

    devices.read((char *)&device_input, sizeof(input_event));

    if (device_input.type != EV_KEY)
      continue;

    // só ir adiante caso seja um valor que foi pressionado do contrario volta a leitura.
    if (device_input.value != 1)
      continue;

    char c = decode_key(device_input.code);

    if (c == -1 && device_input.code != 0) {
      /*
      // mostre caso queira saber um codigo especifico
      std::cout << "\ndevice input code: " << device_input.code << std::endl
                << "Input type: " << device_input.type << std::endl
                << "Input value: " << device_input.value << std::endl;

      */
      continue;
    }

    std::cout << c;

    usleep(500);
  }

  return 0;
}
