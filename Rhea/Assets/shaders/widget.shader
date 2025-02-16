[scene]
type="//meworla/shader"
name="widget"
code="//rhea/shaders/widget.hlsl"
stencil_test="disabled"
alpha_blending=true
material_slots_ps=("BaseColor")
material_slots_vs=("tiling", "scaling")
material_slottypes_vs=("float2", "float2")
