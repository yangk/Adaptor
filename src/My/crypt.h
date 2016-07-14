/*
 * 
 * 文件名称: crypt.h
 *
 * 概述:    SSC1650对外提供的接口说明
 *
 * 当前版本:
 * 主要修改:
 * 完成时间:
 *
 */
 
#ifndef CRYPT_H
#define CRYPT_H

/*
 * 函数说明: uart通信加密函数
 * 输入参数: frm - 数据缓冲区指针
             len - 数据长度
 * 输出参数: 无
 * 返回值:  大于 0 - 加密正确。返回报文长度 -1 - 报文错误
 *
 */
int encrypt (unsigned char *frm, int len);

/*
 * 函数说明: uart通信解密函数
 * 输入参数: frm - 数据缓冲区指针
             len - 数据长度
 * 输出参数: 无
 * 返回值:  大于 0 - 解密正确。返回报文长度 -1 - 报文错误
 *
 */
int decrypt (unsigned char *frm, int len);

/*
 * 函数说明:点抄时等待时间计算函数
 * 输入参数: hops - 节点的跳数
 * 返回值:  返回需要等待的时间
 *
 */
int timeout (int hops);

#endif
