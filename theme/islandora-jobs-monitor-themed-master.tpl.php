<div class="master<?php print (($running) ? ' running' : (($host_record['hostname'] <> 'pa-fed-staff-01') ? ' stopped' : '')); ?>" id="<?php print $host_record['hostname']; ?>"
     <?php if (($host_record['hostname'] <> 'pa-fed-staff-01')) : ?> title="<?php print (($running) ? 'Gearman daemon is running' : 'Gearman daemon is stopped'); ?>"<?php endif; ?>>
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
