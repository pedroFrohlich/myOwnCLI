#include <iostream>
#include <filesystem>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void listar(const std::string& path) {
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    std::cout << entry.path().filename().string() << std::endl;
  }
}

void irPara(const std::string& path) {
  std::filesystem::current_path(path);
  std::cout << "Foi para: " << path << std::endl;
}

void criarPasta(const std::string& path) {
  std::filesystem::create_directory(path);
  std::cout << "Pasta criada: " << path << std::endl;
}

void seLivrar(const std::string& path) {
  std::filesystem::remove_all(path);
  std::cout << "Arquivo removido: " << path << std::endl;
}

void daemonize() {
  pid_t pid;

  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  if (setsid() < 0) {
    exit(EXIT_FAILURE);
  }

  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }
  
  if (chdir("/") < 0) {
    exit(EXIT_FAILURE);
  }

  umask(0);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  open("/dev/null", O_RDONLY);
  open("/dev/null", O_WRONLY);
  open("/dev/null", O_WRONLY);
}

void process_command(const std::string& command, int argc, char** argv) {
  try {
    if (command == "listar") {
      std::string path = (argc > 2) ? argv[2] : ".";
      listar(path);
    } else if (command == "irPara") {
      if (argc < 3) {
        std::cerr << "irPara requer argumentos" << std::endl;
      }
      irPara(argv[2]);
    } else if (command == "criarPasta") {
      if (argc < 3) {
        std::cerr << "criarPasta requer argumentos" << std::endl;
      }
      criarPasta(argv[2]);
    } else if (command == "seLivrar") {
      if (argc < 3) {
        std::cerr << "seLivrar requer argumentos" << std::endl;
      }
      seLivrar(argv[2]);
    } else {
      std::cerr << "Comando desconhecido: " << command << std::endl;
    }
  } catch (const std::filesystem::filesystem_error& e) {
    std::cerr << "Erro: " << e.what() << std::endl;
  }
}

int main(int argc, char** argv) {
  daemonize();
  if (argc < 2) {
    std::cerr << "Uso correto: myCLI <command> [arguments]" << std::endl;
    return 1;
  }

  std::string command = argv[1];
  process_command(command, argc, argv);

  while (true) {
    sleep(10);
  }
  return EXIT_SUCCESS;
}
