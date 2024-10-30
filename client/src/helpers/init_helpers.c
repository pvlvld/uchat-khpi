#include "../../inc/header.h"

t_helpers init_helpers(void) {
  t_helpers helpers = {
      .set_classname_and_id = set_classname_and_id,
      .show_notification = show_notification
  };

  return helpers;
}
