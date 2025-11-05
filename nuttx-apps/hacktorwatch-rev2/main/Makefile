############################################################################
# apps/hacktorwatch-rev2/
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

include $(APPDIR)/Make.defs

PROGNAME = $(CONFIG_HACKTORWATCH_REV2_PROGNAME)
PRIORITY = $(CONFIG_HACKTORWATCH_REV2_PRIORITY)
STACKSIZE = $(CONFIG_HACKTORWATCH_REV2_STACKSIZE)
MODULE = $(CONFIG_HACKTORWATCH_REV2)

MAINSRC = hacktorwatch_main.c display.c imu.c

ifneq ($(CONFIG_FF_DRV2605L),)
MAINSRC += haptic.c
endif

include $(APPDIR)/Application.mk
