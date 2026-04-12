#include <iostream>
#include <cstdlib>

#include <vector>
#include <augr/rack/node.h>

using namespace augr;

int main( int argc, char *argv[] )
{
  Node a, b;
  SlotT<float> output(a, "input1");
  //output.value_.subscribe();
  return 0;
}
