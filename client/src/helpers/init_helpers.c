#include "../../inc/header.h"

t_helpers init_helpers(void) {
  t_helpers helpers = {
      .set_classname_and_id = set_classname_and_id,
      .show_notification = show_notification,
      .add_hover = add_hover,
      .strdup = mx_strdup,
      .create_avatar = create_avatar,
      .create_image = create_image,
  };

  return helpers;
}
