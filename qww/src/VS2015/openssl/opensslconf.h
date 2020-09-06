/** @file       opensslconf.h
 *  @copyright  HangZhou Hikvision System Technology Co., Ltd.
 *              All Right Reserved.
 *  @brief      opensslconf.hͷ�ļ����л��ļ�, ��ͬ�İ汾����ļ���̫һ��, ����
 *              ��һ�²����ͷ�ļ����������ٴ����ͷ�ļ�������
 *
 *  @author     ����  ( fangyu@hikvision.com )
 *
 *  @note       History
 *  @par        02/17/2016 V1.0.0.0 Created by ����
 *  @par        03/15/2016 V1.0.1.0 Modified by ����, ����·���е�б��
 */

/** @note   ԭ����opensslconf.h��û��ͷ�ļ����ظ����ú�, ���ϻ�������, ��������
 *          Ҳ�Ͳ�дͷ�ļ����ظ����ú�
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

