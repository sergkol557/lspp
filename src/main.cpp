#include "lspp.hpp"

int main(int argc, char** argv){
  auto lspp = std::make_unique<Lspp>();
  lspp->run(argc, argv);
}
