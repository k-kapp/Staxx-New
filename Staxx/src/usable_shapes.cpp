
#include "../include/usable_shapes.h"
#include "../include/misc_types.h"

#include <vector>
#include <memory>

using namespace std;

class shape;

std::vector<std::shared_ptr<shape> > selected_shapes;
std::vector<std::shared_ptr<shape> > all_shapes;

void init_imported_shapes()
{
	imported_shapes();
	selected_shapes = imported_shapes::shapes;
	all_shapes = imported_shapes::shapes;
}

