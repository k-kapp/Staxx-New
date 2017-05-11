
#include "../include/usable_shapes.h"

#include <vector>
#include <memory>

using namespace std;


vector<shared_ptr<shape> > usable_shapes = import_shapes();
vector<shared_ptr<shape> > all_shapes = import_shapes();
