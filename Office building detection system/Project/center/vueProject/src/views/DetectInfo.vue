<template>
  <el-row tpye="flex" gutter="10">
    <el-col :span="12">
      <el-card class="env-detect-card">
        <template #header>
          <el-row justify="space-between">
            <el-col class="detect-office" :span="12">
              <span>办公室①</span>
            </el-col>
            <el-col class="existance" :span="4">
              <span class="exist" v-if="exist_1 !== undefined">{{ exist_1.value === 'true'? "在岗" : "离岗" }}</span>
              <span class="exist" v-if="exist_1 == undefined"> --.- </span>
            </el-col>
          </el-row>
        </template>
          <el-row class = "detect-name" justify="space-between"> 
            <el-col :span="12">
              <img src= "/temp.svg" class="detect-icon"/>
              温度 
            </el-col>
            <el-col :span="12">
              <span class="detect-value" v-if="temp_1 !== undefined">{{ temp_1.value }}</span>
              <span class="detect-value" v-if="temp_1 == undefined"> --.- </span> &deg;C 
            </el-col>
          </el-row>
          <el-row class = "detect-name" justify="space-between"> 
            <el-col :span="12">
            <img src= "/humi.svg" class="detect-icon"/>
            湿度 
            </el-col>
            <el-col :span="12">
              <span class="detect-value" v-if="humi_1 !== undefined">{{ humi_1.value }}</span>
              <span class="detect-value" v-if="humi_1 == undefined"> --.- </span> %
            </el-col>
          </el-row>
          <el-row class = "detect-name" justify="space-between">
            <el-col :span="12">
            <img src= "/CO2.svg" class="detect-icon"/> 
            CO<sub>2</sub>浓度 
            </el-col>
            <el-col :span="12">
              <span class="detect-value" v-if="co2_1 !== undefined">{{ co2_1.value }}</span>
              <span class="detect-value" v-if="co2_1 == undefined"> --.--- </span> ppm 
            </el-col>
          </el-row>
          <el-row class = "detect-name" justify="space-between">
            <el-col :span="12">
            <img src= "/CH2O.svg" class="detect-icon"/> 
            甲醛浓度
            </el-col>
            <el-col :span="12">
              <span class="detect-value" v-if="ch2o_1 !== undefined">{{ ch2o_1.value }}</span>
              <span class="detect-value" v-if="ch2o_1 == undefined"> --.--- </span> mg/m<sup>3</sup> 
            </el-col>
          </el-row>
      </el-card>
    </el-col>
    <el-col :span="12">
      <el-card class="env-detect-card">
        <template #header>
          <el-row justify="space-between">
            <el-col class="detect-office" :span="12">
              <span>办公室②</span>
            </el-col>
            <el-col class="existance" :span="4">
              <span class="exist" v-if="exist_2 !== undefined">{{ exist_2.value === 'true' ? "在岗" : "离岗" }}</span>
              <span class="exist" v-if="exist_2 == undefined"> --.- </span>
            </el-col>
          </el-row>
        </template>
          <el-row class = "detect-name" justify="space-between"> 
            <el-col :span="12">
              <img src= "/temp.svg" class="detect-icon"/>
              温度 
            </el-col>
            <el-col :span="12">
              <span class="detect-value" v-if="temp_2 !== undefined">{{ temp_2.value }}</span>
              <span class="detect-value" v-if="temp_2 == undefined"> --.- </span> &deg;C 
            </el-col>
          </el-row>
          <el-row class = "detect-name" justify="space-between"> 
            <el-col :span="12">
            <img src= "/humi.svg" class="detect-icon"/>
            湿度 
            </el-col>
            <el-col :span="12">
              <span class="detect-value" v-if="humi_2 !== undefined">{{ humi_2.value }}</span>
              <span class="detect-value" v-if="humi_2 == undefined"> --.- </span> %
            </el-col>
          </el-row>
          <el-row class = "detect-name" justify="space-between">
            <el-col :span="12">
            <img src= "/CO2.svg" class="detect-icon"/> 
            CO<sub>2</sub>浓度 
            </el-col>
            <el-col :span="12">
              <span class="detect-value" v-if="co2_2 !== undefined">{{ co2_2.value }}</span>
              <span class="detect-value" v-if="co2_2 == undefined"> --.--- </span> ppm
            </el-col>
          </el-row>
          <el-row class = "detect-name" justify="space-between">
            <el-col :span="12">
            <img src= "/CH2O.svg" class="detect-icon"/> 
            甲醛浓度
            </el-col>
            <el-col :span="12">
              <span class="detect-value" v-if="ch2o_2 !== undefined">{{ ch2o_2.value }}</span>
              <span class="detect-value" v-if="ch2o_2 == undefined"> --.--- </span> mg/m<sup>3</sup> 
            </el-col>
          </el-row>
      </el-card>
    </el-col>
  </el-row>
</template>

<script lang="ts" setup>
import { computed, onMounted, watch } from 'vue'
import { temp_1, humi_1, co2_1, ch2o_1, exist_1 } from '@/api/getDevProp'
import { temp_2, humi_2, co2_2, ch2o_2, exist_2 } from '@/api/getDevProp'

// setInterval(() => {
//   console.log(temp_1.value)
// }, 1000)

</script>

<style lang="css" scoped>
.el-card{
    --el-card-border-color: #0a0a0a00 !important;
    --el-card-padding: 3vh !important;
  }
  .env-detect-card {
    border: none;
    background-color: #0100016b;
    border-radius: 15px;
    width: 15vw;
    height: 30vh;
    margin-top: 1vh;
    margin-bottom: 1vh;
  }
  .detect-office {
    display: flex;  
    align-items: flex-end; 
    font-size: 2.5vh;
    margin-bottom: -8px;
    margin-right: 0px;
    color: #fff;
  }
  .exist {
    display: flex;  
    align-items: flex-end; 
    font-size: 16px;
    color: #00ff4c;
    font-weight: 500;
    margin-bottom: -8px;
    margin-right: -8px;
  }
  .detect-name {
    font-size: 16px;
    margin-bottom: 8px;
    align-items: baseline;
    font-weight: 400;
    color: #fff;
  }
  .detect-icon {
    width: 18px;
    height: 18px;
    margin-right: 8px;
  }
  .detect-value {
    font-size: 16px;
    color: #00ff4c;
    font-weight: 200;
    align-items: baseline;
  }
</style>