// =====================================================================================
//	Copyright (C) 2019 by Jiaxing Shao.All rights reserved.
//	文 件 名:  sps_decode.cpp
//	作    者:  Jiaxing Shao, 13315567369@163.com
//	版 本 号:  1.0
//	创建时间:  2019年07月02日 23时35分49秒
//	Compiler:  g++
//	描    述:  
// =====================================================================================

#include "sps_decode.h"
#include "functions.h"

int U(unsigned char *pBuf, int &nStartBit, int nBitSize)
{
	int nValue = 0;
	for(int i = 0; i < nBitSize; i++) {
		nValue = (nValue << 1) + ((pBuf[nStartBit / 8] & (0x80 >> (nStartBit % 8))) ? 1 : 0);
		nStartBit++;
	}
	return nValue;
}

unsigned int Ue(unsigned char *pBuf, int nLen, int &nStartBit)
{
	unsigned int nZeroNum = 0;
	for(int i = 0; i < nLen * 8; i++){
		if(pBuf[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
			break;
		nZeroNum++;
		nStartBit++;
	}

	unsigned int nValue = 0;
	for(int i = 0; i < nZeroNum + 1; i++){
		nValue = (nValue << 1) + ((pBuf[nStartBit / 8] >> (7 - nStartBit % 8)) & 0x01);
		nStartBit++;
	}
	return nValue - 1;
}

int Se(unsigned char *pBuf, int nLen, int &nStartBit)
{
	int UeVal = Ue(pBuf, nLen, nStartBit);
	double k = UeVal;
	int nValue = ceil(k / 2);
	if(UeVal % 2 == 0)
		nValue = -nValue;
	return nValue;
}

void de_emulation_prevention(unsigned char *pBuf, int &nLen)
{
	int s = 0, e = 0;
	for(int i = 0; i < nLen - 2; i++){
		if(0 == (pBuf[i] ^ 0x00) + (pBuf[i + 1] ^ 0x00) + (pBuf[i + 2] ^ 0x03)){
			for(int j = i + 2; j < nLen - 1; j++){
				pBuf[j] = pBuf[j + 1];
			}
			nLen -= 1;
		}
	}
}

int h264_decode_sps(unsigned char *pBuf, int nLen, sps_data_t &sps)
{
	de_emulation_prevention(pBuf, nLen);
	int nStartBit = 0;
	if(pBuf[0] == 0x00 && pBuf[1] == 0x00 && pBuf[2] == 0x00 && pBuf[3] == 0x01)
		nStartBit = 4 * 8;
	else if(pBuf[0] == 0x00 && pBuf[1] == 0x00 && pBuf[2] == 0x01)
		nStartBit = 3 * 8;
	else
		return -1;

	int forbidden_zero_bit = U(pBuf, nStartBit, 1);
	int nal_ref_idc = U(pBuf, nStartBit, 2);
	int nal_unit_type = U(pBuf, nStartBit, 5);
	if(nal_unit_type != 7)
		return -1;

	sps.profile_idc = U(pBuf, nStartBit, 8);
	sps.constraint_set0_flag = U(pBuf, nStartBit, 1);
	sps.constraint_set1_flag = U(pBuf, nStartBit, 1);
	sps.constraint_set2_flag = U(pBuf, nStartBit, 1);
	sps.constraint_set3_flag = U(pBuf, nStartBit, 1);
	sps.constraint_set4_flag = U(pBuf, nStartBit, 1);
	sps.constraint_set5_flag = U(pBuf, nStartBit, 1);
	sps.reserved_zero_2bits = U(pBuf, nStartBit, 2);
	sps.level_id = U(pBuf, nStartBit, 8);
	sps.seq_parameter_set_id = Ue(pBuf, nLen, nStartBit);
	if(sps.profile_idc == 100 || sps.profile_idc == 110 ||
	   sps.profile_idc == 122 || sps.profile_idc == 144)
	{
		sps.chroma_format_idc = Ue(pBuf, nLen, nStartBit);
		if(sps.chroma_format_idc == 3)
			sps.residual_colour_transform_flag = U(pBuf, nStartBit, 1);
		sps.bit_depth_luma_minus8 = Ue(pBuf, nLen, nStartBit);
		sps.bit_depth_chroma_minus8 = Ue(pBuf, nLen, nStartBit);
		sps.qpprime_y_zero_transform_bypass_flag = U(pBuf, nStartBit, 1);
		sps.seq_scaling_matrix_present_flag = U(pBuf, nStartBit, 1);
		if(sps.seq_scaling_matrix_present_flag)
			for(int i = 0; i < 8; i++)
				sps.seq_scaling_list_present_flag[i] = U(pBuf, nStartBit, 1);
	}
	sps.log2_max_frame_num_minus4 = Ue(pBuf, nLen, nStartBit);
	sps.pic_order_cnt_type = Ue(pBuf, nLen, nStartBit);
	if(sps.pic_order_cnt_type == 0){
		sps.log2_max_pic_order_cnt_lsb_minus4 = Ue(pBuf, nLen, nStartBit);
	}else if(sps.pic_order_cnt_type == 1){
		sps.delta_pic_order_always_zero_flag = U(pBuf, nStartBit, 1);
		sps.offset_for_non_ref_pic = Se(pBuf, nLen, nStartBit);
		sps.offset_for_top_to_bottom_field = Se(pBuf, nLen, nStartBit);
		sps.num_ref_frames_in_pic_order_cnt_cycle = Ue(pBuf, nLen, nStartBit);
		sps.offset_for_ref_frame = new int[sps.num_ref_frames_in_pic_order_cnt_cycle];
		for(int i = 0; i < sps.num_ref_frames_in_pic_order_cnt_cycle; i++)
			sps.offset_for_ref_frame[i] = Se(pBuf, nLen, nStartBit);
	}
	sps.num_ref_frames = Ue(pBuf, nLen, nStartBit);
	sps.gaps_in_frame_num_value_allowed_flag = U(pBuf, nStartBit, 1);
	sps.pic_width_in_mbs_minus1 = Ue(pBuf, nLen, nStartBit);
	sps.pic_height_in_map_units_minus1 = Ue(pBuf, nLen, nStartBit);
	sps.frame_mbs_only_flag = U(pBuf, nStartBit, 1);
	if(!sps.frame_mbs_only_flag)
		sps.mb_adaptive_frame_field_flag = U(pBuf, nStartBit, 1);
	sps.direct_8x8_inference_flag = U(pBuf, nStartBit, 1);
	sps.frame_cropping_flag = U(pBuf, nStartBit, 1);
	if(sps.frame_cropping_flag){
		sps.frame_crop_left_offset = Ue(pBuf, nLen, nStartBit);
		sps.frame_crop_right_offset = Ue(pBuf, nLen, nStartBit);
		sps.frame_crop_top_offset = Ue(pBuf, nLen, nStartBit);
		sps.frame_crop_bottom_offset = Ue(pBuf, nLen, nStartBit);
	}
	sps.vui_parameter_present_flag = U(pBuf, nStartBit, 1);
	if(sps.vui_parameter_present_flag)
	{
		sps.aspect_ratio_info_present_flag = U(pBuf, nStartBit, 1);
		if(sps.aspect_ratio_info_present_flag){
			sps.aspect_ratio_idc = U(pBuf, nStartBit, 8);
			if(sps.aspect_ratio_idc == 255){
				sps.sar_width = U(pBuf, nStartBit, 16);
				sps.sar_height = U(pBuf, nStartBit, 16);
			}
		}
		sps.overscan_info_present_flag = U(pBuf, nStartBit, 1);
		if(sps.overscan_info_present_flag)
			sps.overscan_appropriate_flagu = U(pBuf, nStartBit, 1);
		sps.video_signal_type_present_flag = U(pBuf, nStartBit, 1);
		if(sps.video_signal_type_present_flag){
			sps.video_format = U(pBuf, nStartBit, 3);
			sps.video_full_range_flag = U(pBuf, nStartBit, 1);
			sps.colour_description_present_flag = U(pBuf, nStartBit, 1);
			if(sps.colour_description_present_flag){
				sps.colour_primaries = U(pBuf, nStartBit, 8);
				sps.transfer_characteristics = U(pBuf, nStartBit, 8);
				sps.matrix_coefficients = U(pBuf, nStartBit, 8);
			}
		}
		sps.chroma_location_info_present_flag = U(pBuf, nStartBit, 1);
		if(sps.chroma_location_info_present_flag){
			sps.chroma_location_type_top_filed = Ue(pBuf, nLen, nStartBit);
			sps.chroma_location_type_bottom_filed = Ue(pBuf, nLen, nStartBit);
		}
		sps.timing_info_present_flag = U(pBuf, nStartBit, 1);
		if(sps.timing_info_present_flag){
			sps.num_units_in_tick = U(pBuf, nStartBit, 32);
			sps.time_scale = U(pBuf, nStartBit, 32);
		}
	}
	return 0;
}

int h264_decode_pps(unsigned char *pBuf, int nLen, pps_data_t &pps)
{
	de_emulation_prevention(pBuf, nLen);
	int nStartBit = 0;
	if(pBuf[0] == 0x00 && pBuf[1] == 0x00 && pBuf[2] == 0x00 && pBuf[3] == 0x01)
		nStartBit = 4 * 8;
	else if(pBuf[0] == 0x00 && pBuf[1] == 0x00 && pBuf[2] == 0x01)
		nStartBit = 3 * 8;
	else
		return -1;

	int forbidden_zero_bit = U(pBuf, nStartBit, 1);
	int nal_ref_idc = U(pBuf, nStartBit, 2);
	int nal_unit_type = U(pBuf, nStartBit, 5);
	if(nal_unit_type != 8)
		return -1;

	return 0;
}

void h264_print_info(sps_data_t sps, pps_data_t pps)
{
	int width = 0;
	int height = 0;
	int fps = 0;

	width = (sps.pic_width_in_mbs_minus1 + 1) * 16;
	height = (sps.pic_height_in_map_units_minus1 + 1) * 16;
	fps = sps.time_scale / (2 * sps.num_units_in_tick);

	log_debug("*********************************** VIDEO INFO *************************************");
	log_debug("Width     : %d", width);
	log_debug("Height    : %d", width);
	log_debug("FPS       : %d", fps);
	log_debug("*************************************** END ****************************************");
}

void h264_print_sps(sps_data_t sps)
{
	log_debug("*********************************** SPS *************************************");
	log_debug("profile_idc                              : %d", sps.profile_idc);
	log_debug("constraint_set0_flag                     : %d", sps.constraint_set0_flag);
	log_debug("constraint_set1_flag                     : %d", sps.constraint_set1_flag);
	log_debug("constraint_set2_flag                     : %d", sps.constraint_set2_flag);
	log_debug("constraint_set3_flag                     : %d", sps.constraint_set3_flag);
	log_debug("constraint_set4_flag                     : %d", sps.constraint_set4_flag);
	log_debug("constraint_set5_flag                     : %d", sps.constraint_set5_flag);
	log_debug("reserved_zero_2bits                      : %d", sps.reserved_zero_2bits);
	log_debug("level_id                                 : %d", sps.level_id);
	log_debug("seq_parameter_set_id                     : %d", sps.seq_parameter_set_id);
	if(sps.profile_idc == 100 || sps.profile_idc == 110 ||
	   sps.profile_idc == 122 || sps.profile_idc == 144)
	{
		log_debug(    "chroma_format_idc                    : %d", sps.chroma_format_idc);
		if(sps.chroma_format_idc == 3)
			log_debug("        residual_colour_transform_flag: %d", sps.residual_colour_transform_flag);
		log_debug("    bit_depth_luma_minus8                : %d", sps.bit_depth_luma_minus8);
		log_debug("    bit_depth_chroma_minus8              : %d", sps.bit_depth_chroma_minus8);
		log_debug("    qpprime_y_zero_transform_bypass_flag : %d", sps.qpprime_y_zero_transform_bypass_flag);
		log_debug("    seq_scaling_matrix_present_flag      : %d", sps.seq_scaling_matrix_present_flag);
		if(sps.seq_scaling_matrix_present_flag)
			for(int i = 0; i < 8; i++)
				log_debug("        seq_scaling_list_present_flag[%d]: %d", i, sps.seq_scaling_list_present_flag[i]);
	}
	log_debug("log2_max_frame_num_minus4                : %d", sps.log2_max_frame_num_minus4);
	log_debug("pic_order_cnt_type                       : %d", sps.pic_order_cnt_type);
	if(sps.pic_order_cnt_type == 0){
		log_debug("    log2_max_pic_order_cnt_lsb_minus4    : %d", sps.log2_max_pic_order_cnt_lsb_minus4);
	}else if(sps.pic_order_cnt_type == 1){
		log_debug("    delta_pic_order_always_zero_flag     : %d", sps.delta_pic_order_always_zero_flag);
		log_debug("    offset_for_non_ref_pic               : %d", sps.offset_for_non_ref_pic);
		log_debug("    offset_for_top_to_bottom_field       : %d", sps.offset_for_top_to_bottom_field);
		log_debug("    num_ref_frames_in_pic_order_cnt_cycle: %d", sps.num_ref_frames_in_pic_order_cnt_cycle);
		for(int i = 0; i < sps.num_ref_frames_in_pic_order_cnt_cycle; i++)
			log_debug("    offset_for_ref_frame[%d]             : %d", i, sps.offset_for_ref_frame[i]);
	}
	log_debug("num_ref_frames                           : %d", sps.num_ref_frames);
	log_debug("gaps_in_frame_num_value_allowed_flag     : %d", sps.gaps_in_frame_num_value_allowed_flag);
	log_debug("pic_width_in_mbs_minus1                  : %d", sps.pic_width_in_mbs_minus1);
	log_debug("pic_height_in_map_units_minus1           : %d", sps.pic_height_in_map_units_minus1);
	log_debug("frame_mbs_only_flag                      : %d", sps.frame_mbs_only_flag);
	if(sps.frame_mbs_only_flag)
		log_debug("    mb_adaptive_frame_field_flag         : %d", sps.mb_adaptive_frame_field_flag);
	log_debug("direct_8x8_inference_flag                : %d", sps.direct_8x8_inference_flag);
	log_debug("frame_cropping_flag                      : %d", sps.frame_cropping_flag);
	if(sps.frame_cropping_flag){
		log_debug("    frame_crop_left_offset               : %d", sps.frame_crop_left_offset);
		log_debug("    frame_crop_right_offset              : %d", sps.frame_crop_right_offset);
		log_debug("    frame_crop_top_offset                : %d", sps.frame_crop_top_offset);
		log_debug("    frame_crop_bottom_offset             : %d", sps.frame_crop_bottom_offset);
	}
	log_debug("vui_parameter_present_flag               : %d", sps.vui_parameter_present_flag);
	if(sps.vui_parameter_present_flag)
	{
		log_debug("    aspect_ratio_info_present_flag       : %d", sps.aspect_ratio_info_present_flag);
		if(sps.aspect_ratio_info_present_flag){
			log_debug("        aspect_ratio_idc                 : %d", sps.aspect_ratio_idc);
			if(sps.aspect_ratio_idc){
				log_debug("            sar_width                    : %d", sps.sar_width);
				log_debug("            sar_height                   : %d", sps.sar_height);
			}
		}
		log_debug("    overscan_info_present_flag           : %d", sps.overscan_info_present_flag);
		if(sps.overscan_info_present_flag)
			log_debug("        overscan_appropriate_flagu       : %d", sps.overscan_appropriate_flagu);
		log_debug("    video_signal_type_present_flag       : %d", sps.video_signal_type_present_flag);
		if(sps.video_signal_type_present_flag){
			log_debug("        video_format                     : %d", sps.video_format);
			log_debug("        video_full_range_flag            : %d", sps.video_full_range_flag);
			log_debug("        colour_description_present_flag  : %d", sps.colour_description_present_flag);
			if(sps.colour_description_present_flag){
				log_debug("            colour_primaries             : %d", sps.colour_primaries);
				log_debug("            transfer_characteristics     : %d", sps.transfer_characteristics);
				log_debug("            matrix_coefficients          : %d", sps.matrix_coefficients);
			}
		}
		log_debug("    chroma_location_info_present_flag    : %d", sps.chroma_location_info_present_flag);
		if(sps.chroma_location_info_present_flag){
			log_debug("        chroma_location_type_top_filed   : %d", sps.chroma_location_type_top_filed);
			log_debug("        chroma_location_type_bottom_filed: %d", sps.chroma_location_type_bottom_filed);
		}
		log_debug("    timing_info_present_flag             : %d", sps.timing_info_present_flag);
		if(sps.timing_info_present_flag){
			log_debug("        num_units_in_tick                : %d", sps.num_units_in_tick);
			log_debug("        time_scale                       : %d", sps.time_scale);
		}
	}
	log_debug("*********************************** END *************************************");
}

void h264_print_pps(pps_data_t pps)
{
}

