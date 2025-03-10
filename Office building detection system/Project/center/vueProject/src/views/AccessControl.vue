<template>
  <el-row class="access-control">  
    <el-col :span="12">
      <el-scrollbar class="record-scrollbar">
        <el-timeline>
          <el-timeline-item
            v-for="(activity, index) in reversedActivities"
            :key="index"
            :timestamp="activity.timestamp"
            :color="activity.color"
          >
            {{ activity.content }}
          </el-timeline-item>
        </el-timeline>  
      </el-scrollbar>
    </el-col>
    <el-col :span="12" class="pic-container">
      <div class="pic"></div>
    </el-col>
  </el-row>
</template>

<script lang="ts" setup>
import { ref, watch } from 'vue';
import { accessInfo } from '@/api/getCenterProp';

interface Activity {
  content: string;
  timestamp: string;
  color: string;
}

const activities = ref<Array<Activity>>([
  {
    content: '欢迎使用',
    timestamp: new Date().toLocaleString(),
    color: '#0080ff', 
  },
]);

  import { computed } from 'vue';

const reversedActivities = computed(() => {
  return activities.value.slice().reverse();
});

watch(accessInfo, (newValue, oldValue) => {
  if (newValue && oldValue){
    if(newValue.time !== oldValue.time) {
      const newActivity: Activity = {
        content: newValue.value === 'Unknow' ? '未知访客' : '欢迎, ' + newValue.value,
        timestamp: new Date(newValue.time).toLocaleString(),
        color: newValue.value === 'Unknow' ? 'red' : 'lightgreen', // 可以根据实际情况调整颜色
      };
      activities.value.push(newActivity);
    }
  }
}, { immediate: false });

</script>


<style scoped>
  .el-timeline {  
    font-size: 2vh;    
    --el-text-color-primary: #fff;
    --el-text-color-secondary: #bebebe;
  }
  
  .el-timeline-item__content {  
    color: #fff !important;; 
    font-size: 20px;
    
  }  
    
  .el-timeline-item__timestamp {  
    color: #fff !important;
    font-size: 14px; 
  }  

  .access-control {
    display: flex;
    border-radius: 15px;  
    border-color: transparent;    
    margin-top: 1vh;
    margin-bottom: 1vh;
    background-color: #0100016b;
    padding-top: 3vh;
    padding-bottom: 3vh;
    height: 35vh;
    width: 39vw;
  }

  .record-scrollbar {
    margin-top: 3vh;
    margin-left: 1vw;
    /* padding-bottom: 3vh; */
    height: 25vh;
    width: 80%;
  }

  .pic-container {
    margin-left: -3vw;
    height: 100%;
  }

  .pic {
    height: 100%;
    width: 100%;
    border-radius: 15px;
    background-color: #0100016b;
  }
</style>