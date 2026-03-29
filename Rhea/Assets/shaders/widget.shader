[scene]
type="//meworla/shader"
name="widget"
code_hlsl="//rhea/shaders/widget.hlsl"
code_glsl_vs="//rhea/shaders/widget_vs.glsl"
code_glsl_fs="//rhea/shaders/widget_fs.glsl"
stencil_test="disabled"
alpha_blending=true
depth_test=false
depth_write=false
cull_mode="none"
material_slots_ps=("BaseColor")
material_slots_vs=("tiling", "scaling")
material_slottypes_vs=("float2", "float2")
