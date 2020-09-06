/** @file       opensslconf.h
 *  @copyright  HangZhou Hikvision System Technology Co., Ltd.
 *              All Right Reserved.
 *  @brief      opensslconf.h头文件的切换文件, 不同的版本这个文件不太一样, 这里
 *              做一下差异的头文件包含，减少打包中头文件的数量
 *
 *  @author     方宇  ( fangyu@hikvision.com )
 *
 *  @note       History
 *  @par        02/17/2016 V1.0.0.0 Created by 方宇
 *  @par        03/15/2016 V1.0.1.0 Modified by 方宇, 修正路径中的斜杠
 */

/** @note   原生的opensslconf.h中没有头文件防重复引用宏, 加上会有问题, 所以这里
 *          也就不写头文件防重复引用宏
 */

#if defined( _WIN32 ) && !defined( _WIN64 )
#   include "platform/opensslconf_win32.h"
#elif defined( _WIN32 ) && defined( _WIN64 )
#   include "platform/opensslconf_win64.h"
#endif

#ifdef __linux
#   if defined( __x86_64__ )
#      include "platform/opensslconf_centos_x86_64.h"
#   endif
#endif

