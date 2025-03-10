<template>
  <div>
    <el-row gutter="10" style="margin-top: 3vh;">
      <el-col :span="8" style="margin-left: 3.5vw;"> <DetectInfo /> </el-col>    
      <el-col :span="8"> <AccessControl /> </el-col>
      <el-col :span="5" :offset="2" style="margin-top: 1vh;"> <DeviceFire /> </el-col>
    </el-row>
    <el-row gutter="10">
      <el-col :span="8" style="margin-left: 3.5vw;"> <ChartCO2_1 /> </el-col>   
      <el-col :span="8" style="margin-top: 5vh;"> <ChartTemp_1/> </el-col> 
      <el-col :span="5" :offset="2" style="margin-top: -11vh;"> <DeviceLight /> </el-col>
    </el-row>
    <el-row gutter="10">
      <el-col :span="8" style="margin-left: 3.5vw;"> <ChartCO2_2 /> </el-col>   
      <el-col :span="8" style="margin-top: 5vh;"> <ChartTemp_2/> </el-col> 
      <el-col :span="5" :offset="2" style="margin-top: -28vh;"> <DeviceAir /> </el-col>
    </el-row>
    <el-row gutter="10">
      <el-col :span="8" style="margin-left: 3.5vw;">  </el-col>   
      <el-col :span="8" style="margin-top: 5vh;">  </el-col> 
      <el-col :span="5" :offset="2" style="margin-top: -45vh;"> <DeviceFan /> </el-col>
    </el-row>
  </div>
</template>


<script lang="ts" setup>
import DetectInfo from './DetectInfo.vue'
import ChartTemp_1 from './ChartTemp_1.vue'
import ChartTemp_2 from './ChartTemp_2.vue'
import ChartCO2_1 from './ChartCO2_1.vue'
import ChartCO2_2 from './ChartCO2_2.vue'
import AccessControl from './AccessControl.vue'
import DeviceFire from './DeviceFire.vue'
import DeviceAir from './DeviceAir.vue'
import DeviceFan from './DeviceFan.vue'
import DeviceLight from './DeviceLight.vue'

</script>
    
<style scoped lang="scss">
  .el-card{
    --el-card-border-color: #0a0a0a00 !important;
    --el-card-padding: 3vh !important;

  }
  .linechart {
    background-color: #0100016b;
    border-radius: 15px;
    height: 35vh;
    // width: 3vh;
    padding-top: 1vh;
    padding-bottom: 1vh;
    padding-left: 1vw;
    padding-right: 1vw;
    margin-top: -3vh;
    margin-right: 1vw;
  }
</style>
    