<div class="worker<?php print (($running) ? ' running' : ' stopped'); ?>" id="<?php print $host_record['hostname']; ?>" title="<?php print (($running) ? 'Gearman workers are running' : 'Gearman workers are stopped'); ?>">
  <div class="gauge_wrapper"><?php print $gauge; ?></div>
  <br style="clear:top">
  <?php unset($host_record['hostname']); ?>
  <?php unset($host_record['running']); ?>
  <?php foreach ($host_record as $key => $value): ?>
   <?php if ($key <> 'gearman_running'): ?>
    <div class="div_label"><?php print str_replace("_", " ", ucwords($key)); ?></div>
    <div class="div_value">
<?php print (($key == 'name') ? '<b style="font-size:110%">' : ''); ?>
     <?php print $value; ?>
<?php print (($key == 'name') ? '</b>' : ''); ?>
    </div>
    <br class="clearfix">
   <?php endif; ?>
  <?php endforeach; ?>

  <div class="cpu_graph"><?php print $cpu_values; ?></div>
  <div class="memory_graph"><?php print $memory_values; ?></div>
  
  <?php /* print $control_links; */ ?>
</div>
