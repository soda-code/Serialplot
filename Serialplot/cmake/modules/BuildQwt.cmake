#
# Copyright © 2025 Hasan Yavuz Özderya
#
# This file is part of serialplot.
#
# serialplot is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# serialplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
#

include(ExternalProject)

ExternalProject_Add(QWT
  PREFIX qwt
  URL https://sourceforge.net/projects/qwt/files/qwt/6.3.0/qwt-6.3.0.tar.bz2
  # enable static build and disable unwanted components
  PATCH_COMMAND sed -i -r -e "s/QWT_CONFIG\\s*\\+=\\s*QwtDll/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtPolar/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtWidgets/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtSvg/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtOpenGL/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtDesigner/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtDesignerSelfContained/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtExamples/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtPlayground/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtTests/#&/"
						  -e "s|QWT_INSTALL_PREFIX\\s*=.*|QWT_INSTALL_PREFIX = <INSTALL_DIR>|"
                             <SOURCE_DIR>/qwtconfig.pri
                          -e "s/(CONFIG\\s*\\+=\\s*)debug_and_release/\\1release/"
                          -e "s/(CONFIG\\s*\\+=\\s*)build_all/#&/"
                             <SOURCE_DIR>/qwtbuild.pri
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND qmake6 <SOURCE_DIR>/qwt.pro
  )

ExternalProject_Get_Property(QWT install_dir)
set(QWT_ROOT ${install_dir})
set(QWT_LIBRARY ${QWT_ROOT}/lib/libqwt.a)
set(QWT_INCLUDE_DIR ${QWT_ROOT}/include)
