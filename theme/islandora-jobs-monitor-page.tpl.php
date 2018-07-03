<div class="masters">
  <label>Master/s</label>
  <?php foreach ($themed_hosts['master'] as $themed_host): ?>
    <?php echo $themed_host;?>
  <?php endforeach; ?>
</div>

<div class="workers">
  <label>Worker/s</label>
  <?php foreach ($themed_hosts['workers'] as $themed_host): ?>
    <?php echo $themed_host;?>
  <?php endforeach; ?>
</div>
<div id="tabs" class="tab-report">
  <ul>
    <li><a href="#tabs-1">Jobs Records</a></li>
    <li><a href="#tabs-2">Gearman Queue (<?php print number_format($queue_count); ?>)</a></li>
    <li><a href="#tabs-3">Completed Objects</a></li>
    <li><a href="#tabs-4">Recent Transactions</a></li>
  </ul>
  <div id="tabs-1" class="monitor_report">
    <p>Displaying the 500 most recent jobs.</p>
    <?php print $current_report; ?>
  </div>
  <div id="tabs-2" class="tab_page"><?php print $gearman_queue; ?></div>
  <div id="tabs-3" class="tab_page">
    <p>Displaying the 500 most recent ingested objects.</p>
    <?php print $completed_objects; ?>
  </div>
  <div id="tabs-4" class="tab_page">
    <p>Displaying the 1,000 most recent transactions.</p>
    <?php print $recent_transactions; ?>
  </div>
</div>