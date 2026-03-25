#include "app.h"

using namespace augr;

int main(int, char**) {
  App& app = *new MyApp();

  app.Run();
}