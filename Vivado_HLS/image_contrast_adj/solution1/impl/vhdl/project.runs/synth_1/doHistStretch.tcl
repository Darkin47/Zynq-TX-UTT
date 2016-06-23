# 
# Synthesis run script generated by Vivado
# 

set_param project.defaultXPMLibraries XPM_MEMORY
set_msg_config -id {HDL 9-1061} -limit 100000
set_msg_config -id {HDL 9-1654} -limit 100000
create_project -in_memory -part xc7z020clg484-1

set_param project.singleFileAddWarning.threshold 0
set_param project.compositeFile.enableAutoGeneration 0
set_param synth.vivado.isSynthRun true
set_msg_config -source 4 -id {IP_Flow 19-2162} -severity warning -new_severity info
set_property webtalk.parent_dir /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/project.cache/wt [current_project]
set_property parent.project_path /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/project.xpr [current_project]
set_property default_lib xil_defaultlib [current_project]
set_property target_language VHDL [current_project]
read_ip -quiet /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/project.srcs/sources_1/ip/doHistStretch_ap_fdiv_14_no_dsp_32/doHistStretch_ap_fdiv_14_no_dsp_32.xci
set_property is_locked true [get_files /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/project.srcs/sources_1/ip/doHistStretch_ap_fdiv_14_no_dsp_32/doHistStretch_ap_fdiv_14_no_dsp_32.xci]

read_ip -quiet /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/project.srcs/sources_1/ip/doHistStretch_ap_fmul_2_max_dsp_32/doHistStretch_ap_fmul_2_max_dsp_32.xci
set_property is_locked true [get_files /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/project.srcs/sources_1/ip/doHistStretch_ap_fmul_2_max_dsp_32/doHistStretch_ap_fmul_2_max_dsp_32.xci]

read_ip -quiet /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/project.srcs/sources_1/ip/doHistStretch_ap_sitofp_4_no_dsp_32/doHistStretch_ap_sitofp_4_no_dsp_32.xci
set_property is_locked true [get_files /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/project.srcs/sources_1/ip/doHistStretch_ap_sitofp_4_no_dsp_32/doHistStretch_ap_sitofp_4_no_dsp_32.xci]

read_vhdl -library xil_defaultlib {
  /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/doHistStretch_fmul_32ns_32ns_32_4_max_dsp.vhd
  /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/doHistStretch_CTRL_BUS_s_axi.vhd
  /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/doHistStretch_sitofp_32s_32_6.vhd
  /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/doHistStretch_fdiv_32ns_32ns_32_16.vhd
  /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/doHistStretch.vhd
}
foreach dcp [get_files -quiet -all *.dcp] {
  set_property used_in_implementation false $dcp
}
read_xdc /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/doHistStretch.xdc
set_property used_in_implementation false [get_files /home/darkin/Workspace/Vivado_HLS/image_contrast_adj/solution1/impl/vhdl/doHistStretch.xdc]

read_xdc dont_touch.xdc
set_property used_in_implementation false [get_files dont_touch.xdc]

synth_design -top doHistStretch -part xc7z020clg484-1 -no_iobuf -mode out_of_context


write_checkpoint -force -noxdef doHistStretch.dcp

catch { report_utilization -file doHistStretch_utilization_synth.rpt -pb doHistStretch_utilization_synth.pb }