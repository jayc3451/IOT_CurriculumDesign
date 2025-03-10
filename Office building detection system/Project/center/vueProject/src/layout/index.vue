<template>
  <link rel="icon" href="" type="image/x-icon">
  <div class="common-layout">
    <el-container class="container">
      <el-header>
        <el-row class="header" justify=end>
          <el-col class="header-menu" :span="1">
            <el-dropdown>
              <el-icon :size="24" >
                <setting />
              </el-icon>
              <template #dropdown>
                <el-dropdown-menu>
                  <el-dropdown-item>用户</el-dropdown-item>
                  <el-dropdown-item>设置</el-dropdown-item>
                  <el-dropdown-item>注销</el-dropdown-item>
                </el-dropdown-menu>
              </template>
            </el-dropdown> 
          </el-col>
          <el-col class="header-info" :span="1">
            <el-avatar> Zxp </el-avatar>
            <span class="header-username">Zxp</span>
          </el-col>
        </el-row>
      </el-header>
      <!-- 好看的进度条 -->
      <el-scrollbar>    
        <!-- <div class="scrollbar-content">   -->
          <el-main class="main"><router-view /></el-main>
        <!-- </div>   -->
      </el-scrollbar>  
    </el-container>
  </div>
</template>  


<script lang="ts" setup>
import { Menu as IconMenu, Setting } from '@element-plus/icons-vue'

</script>


<style scoped lang="scss">
.common-layout {
  background-image: url("/background.jpg");
  background-size: cover; 
  background-position: top;; 
}

.container {
  height: 98vh;
  
  .el-header { 
    padding: 0;   
    padding-bottom: 5px;
    height: 32px;         // 父类头部高度
  }
  .scrollbar-content {  
    overflow: hidden;  
  }  
  .header {
    height: 32px;
    width: 100%;
    display: flex;  
    // justify-content: flex-end; // 水平方向靠右对齐  
    // align-items: center;       // 垂直方向居中对齐  
    background: #1d2450;
    border-radius: 4px;       // 圆角边框
  }

  .header-menu {
    display:flex; 
    align-items: center; 
    margin-right: -30px;
  }
  .header-info {
    display:flex; 
    align-items: center; 
    margin-right: 0px;
  }
  .el-avatar{
    --el-avatar-text-size: 9px;
    --el-avatar-size: 28px;
  }
  .header-username {
    display:flex; 
    align-items: center;
    margin-left: 10px;
    color: #ccc6c6;
    font-size: 12px;
  }
}
</style>