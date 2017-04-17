################################################################################
#
# stopwatch 
#
################################################################################

STOPWATCH_VERSION = 1.0
STOPWATCH_SITE = $(TOPDIR)/../src/stopwatch
STOPWATCH_SITE_METHOD = local

define STOPWATCH_BUILD_CMDS
   $(MAKE) $(TARGET_CONFIGURE_OPTS) stopwatch -C $(@D)
endef
define STOPWATCH_INSTALL_TARGET_CMDS 
   $(INSTALL) -D -m 0755 $(@D)/stopwatch $(TARGET_DIR)/usr/bin 
endef

$(eval $(generic-package))
